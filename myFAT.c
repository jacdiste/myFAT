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
        printf("Error: Parent directory is full\n");
        return;
    }

    int freeBlock = findFree(fs->FATfs);
    if(freeBlock == -1){
        printf("Error: No free blocks\n");
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

    fs->currentDir->numFiles++;
    fs->FATfs->FAT[freeBlock] = EF;

    printf("File %s created. \n", name);
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
    printf("Error: File %s not found. \n", name);
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

    printf("Error: File %s not found. Try changing Directory. \n", name);
    return NULL;
}

void closeFile(FileSystem* fs, FileHandle* fh){
    free(fh);
}

void writeFile(FileSystem* fs, FileHandle *fh, const char *buf, int len){

    if(fh==NULL){
        printf("Error: Invalid file. \n");
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

    //controllare se le size sono corrette!!
    fh->pos += len;
    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->startBlock * BLOCK_SIZE]);
    if (fh->pos > file->size) {
        int sizeDiff = fh->pos - file->size;

        file->size = fh->pos;

        DirEntry* parentDir = (DirEntry*)(&fs->FATfs->data[file->parentDirBlock * BLOCK_SIZE]);
        parentDir->size += sizeDiff;
    }
}

void readFile(FileSystem* fs, FileHandle *fh, char *buf, int len){

    if(fh==NULL){
        printf("Error: Invalid file. \n");
        return;
    }

    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->startBlock * BLOCK_SIZE]);
    int currentBlock = fh->startBlock;
    int fileOffset = fh->pos % BLOCK_SIZE;
    int bytesToRead = len;
    int bufOffset = 0;

    while(bytesToRead > 0 && fh->pos < file->size){
        int inBlockReadableBytes = BLOCK_SIZE - fileOffset;

        if(inBlockReadableBytes > bytesToRead){
            inBlockReadableBytes = bytesToRead;
        }

        if(inBlockReadableBytes > file->size - fh->pos){
            inBlockReadableBytes = file->size - fh->pos;
        }

        memcpy(&buf[bufOffset], &fs->FATfs->data[currentBlock * BLOCK_SIZE + fileOffset], inBlockReadableBytes);

        bytesToRead -= inBlockReadableBytes;
        bufOffset += inBlockReadableBytes;
        fh->pos += inBlockReadableBytes;
        fileOffset = (fileOffset + inBlockReadableBytes) % BLOCK_SIZE;

        if(fileOffset==0){
            currentBlock = fs->FATfs->FAT[currentBlock];
        }
    }
}

void seekFile(FileSystem* fs, FileHandle *fh, int newPos){

    if(fh==NULL){
        printf("Error: Invalid file. \n");
        return;
    }

    if(newPos > fh->pos || newPos < 0){
        printf("Error: Invalid seek position. \n");
        return;
    }

    int indexBlock = fh->startBlock;

    int pos = newPos;

    while(pos>=BLOCK_SIZE){
        indexBlock = fs->FATfs->FAT[indexBlock];
        if(indexBlock==EF){
            printf("Error: Seek position out of file bounds. \n");
            return;
        }

        pos -= BLOCK_SIZE;
    }

    fh->pos = newPos;
}

void createDir(FileSystem* fs, const char *dirname){
    DirEntry* parentDir = fs->currentDir;

    if(parentDir->numDir + parentDir->numFiles >= MAX_ENTRIES){
        printf("Error: Current directory is full\n");
        return;
    }

    int freeBlock = findFree(fs->FATfs);

    if(freeBlock == -1){
        printf("Error: No free blocks\n");
        return;
    }

    DirEntry* newDir = (DirEntry*)(&fs->FATfs->data[freeBlock * BLOCK_SIZE]);
    strcpy(newDir->name, dirname);
    newDir->startBlock = freeBlock;
    newDir->size = 0;
    newDir->parentDirBlock = parentDir->startBlock;
    newDir->isDir = 1;
    newDir->numFiles = 0;
    newDir->numDir = 0;
    //spreco di memoria? ->
    //memset(newDir->entries, FREE, sizeof(newDir->entries));

    //for max entries trova blocco libero

    parentDir->numDir++;
    fs->FATfs->FAT[freeBlock] = EF;
}

