#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "myFAT.h"


FileSystem* loadFS(const char* name){
    FileSystem* fs = (FileSystem*)malloc(sizeof(FileSystem));
    
    int fd = open(name, O_RDWR | O_CREAT, 0666);

    if(fd == -1){
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    FATFileSystem* FATfs = mmap(NULL, sizeof(FATFileSystem), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    fs->FATfs = FATfs;

    int fsSize = lseek(fd, 0, SEEK_END);
    if(fsSize < sizeof(FATFileSystem)){
        if(ftruncate(fd, sizeof(FATFileSystem)) == -1){
            perror("Error resizing");
            close(fd);
            free(fs);
            exit(EXIT_FAILURE);
        }
        fsSize = sizeof(FATFileSystem);
    }
    lseek(fd, 0, SEEK_SET);

    int used = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);


        memset(FATfs->FAT, FREE , sizeof(FATfs->FAT));
        memset(FATfs->data, '\0', sizeof(FATfs->data));

        DirEntry* root = (DirEntry*)(&FATfs->data[0]);
        strcpy(root->name, "/");
        root->startBlock = 0;
        root->size = 0;
        root->parentDirBlock = -1;
        root->isDir = 1;
        root->numFiles = 0;
        root->numDir = 0;
        // forse troppo dispendiosa? ->
        memset(root->entries, FREE, sizeof(root->entries));

        fs->currentDir = root;
        FATfs->FAT[0] = EF;
+

    close(fd);
    return fs;
}

void unloadFS(FileSystem* fs){
    if(fs==NULL) return;

    if(fs->FATfs != NULL){
        if(munmap(fs->FATfs, sizeof(FATFileSystem)) == -1){
            perror("Error unmapping FAT File System");
            exit(EXIT_FAILURE);
        }
    }

    free(fs);
}

int findFree(FATFileSystem* FATfs){
    for(int i = 0; i < BLOCKS_NUMBER; i++){
        if(FATfs->FAT[i] == FREE){
            return i;
        }
    }
    return -1;
}

void createFile(FileSystem* fs, const char* name){
    DirEntry* parentDir = fs->currentDir;

    if(parentDir->numFiles + parentDir->numDir >= MAX_ENTRIES){
        printf("Parent directory is full\n");
        return;
    }

    int freeBlock = findFree(fs->FATfs);
    if(freeBlock == -1){
        printf("No free blocks\n");
        return;
    }

    DirEntry* newFile = (DirEntry*)(&fs->FATfs->data[freeBlock * BLOCK_SIZE]);
    strcpy(newFile->name, name);
    newFile->startBlock = freeBlock;
    newFile->size = 0;
    newFile->parentDirBlock = parentDir->startBlock;
    newFile->isDir = 0;
    newFile->numFiles = 0;
    newFile->numDir = 0;
    //spreco di memoria? ->
    //memset(newFile->entries, FREE, sizeof(newFile->entries));

    for(int i = 0; i < MAX_ENTRIES; i++){
        if(fs->currentDir->entries[i] == FREE){
            fs->currentDir->entries[i] = freeBlock;
            break;
        }
    }

    fs->currentDir->numFiles++;
    fs->FATfs->FAT[freeBlock] = EF;
}

void eraseFile(FileSystem* fs, const char* name){
    DirEntry* parentDir = fs->currentDir;

    for(int i = 0; i < MAX_ENTRIES; i++){
        if(parentDir->entries[i] != FREE){
            DirEntry* entry = (DirEntry*)(&fs->FATfs->data[parentDir->entries[i] * BLOCK_SIZE]);
            if(strcmp(entry->name, name) == 0 && !entry->isDir){
                int currentBlock = entry->startBlock;
                while(currentBlock != EF){
                    int nextBlock = fs->FATfs->FAT[currentBlock];
                    fs->FATfs->FAT[currentBlock] = FREE;
                    memset(&fs->FATfs->data[currentBlock * BLOCK_SIZE], '\0', BLOCK_SIZE);
                    currentBlock = nextBlock;
                }
                fs->currentDir->entries[i] = FREE;
                fs->currentDir->numFiles--;
                printf("File %s erased. \n", name);
                return;
            }
        }
    }
    printf("File %s not found. \n", name);
}

FileHandle* openFile(FileSystem* fs, const char* name){
    DirEntry* parentDir = fs->currentDir;

    for(int i = 0; i < MAX_ENTRIES; i++){
        if(parentDir->entries[i]!= FREE){
            DirEntry* entry = (DirEntry*)(&fs->FATfs->data[parentDir->entries[i] * BLOCK_SIZE]);
            if(strcmp(entry->name, name) == 0 && !entry->isDir){
                FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
                fh->currentDir = parentDir;
                fh->startBlock = entry->startBlock;
                fh->pos = 0;
                return fh;
            }
        }
    }

    printf("File %s not found. Try changing Directory. \n", name);
    return NULL;
}

void closeFile(FileSystem* fs, FileHandle* fh){
    free(fh);
}

void writeFile(FileSystem* fs, FileHandle *fh, const char *buf, int len){

    if(fh==NULL){
        printf("Invalid file. \n");
        return;
    }

    int currentBlock = fh->startBlock;
    int fileOffset = fh->pos % BLOCK_SIZE;
    int bytesToWrite = len;
    int bufOffset = 0;

    while(bytesToWrite > 0){
        if(fileOffset == 0){
            if(fs->FATfs->FAT[currentBlock] == EF){
                int newBlock = findFree(fs->FATfs);
                if(newBlock == -1){
                    printf("No more free blocks. \n");
                    return;
                }

                fs->FATfs->FAT[currentBlock] = newBlock;
                fs->FATfs->FAT[newBlock] = EF;
                currentBlock = newBlock;
            }

            else{
                currentBlock = fs->FATfs->FAT[currentBlock];
            }
        }

        int inBlockWritableBytes = BLOCK_SIZE - fileOffset;
        if(inBlockWritableBytes > bytesToWrite){
            inBlockWritableBytes = bytesToWrite;
        }

        memcpy(&fs->FATfs->data[currentBlock * BLOCK_SIZE + fileOffset], &buf[bufOffset], inBlockWritableBytes);

        bytesToWrite -= inBlockWritableBytes;
        bufOffset += inBlockWritableBytes;
        fileOffset = (fileOffset + inBlockWritableBytes) % BLOCK_SIZE;

        if(fileOffset==0 && fs->FATfs->FAT[currentBlock]==EF){
            int newBlock = findFree(fs->FATfs);
            if(newBlock == -1){
                printf("No more free blocks, only %d bytes have been written! \n", len-bytesToWrite);
                fh->pos += (len - bytesToWrite);
                return;
            }

            fs->FATfs->FAT[currentBlock] = newBlock;
            fs->FATfs->FAT[newBlock] = EF;
            currentBlock = newBlock;
        }
    }

    fh->pos += len;
    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->startBlock * BLOCK_SIZE]);
    if (fh->pos > file->size) {
        int sizeDiff = fh->pos - file->size;

        file->size = fh->pos;

        DirEntry* parentDir = (DirEntry*)(&fs->FATfs->data[file->parentDirBlock * BLOCK_SIZE]);
        parentDir->size += sizeDiff;
    }
}

