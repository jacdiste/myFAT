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

    int usedLen = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    if(usedLen > 0){
        fs->FATfs = (FATFileSystem*)mmap(NULL, usedLen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        fs->currentDir = (DirEntry*)(&fs->FATfs->data[0]);    
    }

    else{
        if(ftruncate(fd, sizeof(FATFileSystem)) == -1){
            perror("Error resizing");
            close(fd);
            free(fs);
            exit(EXIT_FAILURE);
        }

        FATFileSystem* FATfs = mmap(NULL, sizeof(FATFileSystem), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        fs->FATfs = FATfs;

        memset(FATfs->FAT, FREE , sizeof(FATfs->FAT));
        memset(FATfs->data, '\0', sizeof(FATfs->data));

        DirEntry* root = (DirEntry*)(&FATfs->data[0]);
        strcpy(root->name, "/");
        root->startDataBlock = -1;
        root->size = 40;
        root->dirBlock = 0;
        root->parentDirBlock = -1;
        root->isDir = 1;
        root->numEntries = 0;

        fs->currentDir = root;
        FATfs->FAT[0] = EF;
    }

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

FileHandle* openFile(FileSystem* fs, const char* name){
    DirEntry* currentDir = fs->currentDir;
    int dirLastBlock = currentDir->startDataBlock;
    while (dirLastBlock != EF) {
        DirEntry* entry = (DirEntry*)(&fs->FATfs->data[dirLastBlock * BLOCK_SIZE]);
        if (strcmp(entry->name, name) == 0 && !entry->isDir) {
            FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
            strcpy(fh->name, name);
            fh->currentDataBlock = entry->startDataBlock;
            fh->currentFatBlock = dirLastBlock;
            fh->pos = 0; //ATTENZIONE OGNI VOLTA CHE APRO UN FILE SI SETTA A 0 pos QUINDI COSA SUCCEDE SE SCRIVO IN MOMENTI DIVERSI?
            return fh;
        }
        dirLastBlock = fs->FATfs->FAT[dirLastBlock];
    }

    printf("Error: File '%s' not found. Try changing Directory. \n", name);
    return NULL;
}

void closeFile(FileSystem* fs, FileHandle* fh){
    if(fh!=NULL){
        free(fh);
    }
}

void createFile(FileSystem* fs, const char* name){
    DirEntry* currentDir = fs->currentDir;
    int dirStartBlock = currentDir->dirBlock;

    if(currentDir->numEntries >= MAX_ENTRIES){
        printf("Error: Parent directory is full\n");
        return;
    }

    while(dirStartBlock != EF){
        DirEntry* entry = (DirEntry*)(&fs->FATfs->data[dirStartBlock * BLOCK_SIZE]);
        if(strcmp(entry->name, name) == 0){
            printf("Error: File '%s' already exists\n", name);
            return;
        }
        dirStartBlock = fs->FATfs->FAT[dirStartBlock];
    }

    int freeBlock = findFree(fs->FATfs);
    if(freeBlock == -1){
        printf("Error: No free blocks\n");
        return;
    }

    DirEntry* newFile = (DirEntry*)(&fs->FATfs->data[freeBlock * BLOCK_SIZE]);
    strcpy(newFile->name, name);
    //
    newFile->startDataBlock = -1;
    newFile->size = 40;
    newFile->dirBlock = freeBlock;
    newFile->parentDirBlock = currentDir->dirBlock;
    newFile->isDir = 0;
    newFile->numEntries = -1;
    
    currentDir->size += newFile->size;
    currentDir->numEntries++;
    
    int lastBlock = currentDir->dirBlock;
    while (fs->FATfs->FAT[lastBlock] != EF) {
        lastBlock = fs->FATfs->FAT[lastBlock];
    }
    fs->FATfs->FAT[lastBlock] = freeBlock;
    fs->FATfs->FAT[freeBlock] = EF;

    printf("File '%s' created. \n", name);
}

void eraseFile(FileSystem* fs, const char* name){
    DirEntry* currentDir = fs->currentDir;
    int entryBlock = currentDir->dirBlock;
    int prevBlock = currentDir->dirBlock;

    while(entryBlock!=EF){
        DirEntry* entry = (DirEntry*)(&fs->FATfs->data[entryBlock * BLOCK_SIZE]);
        if(strcmp(entry->name, name) == 0 && !entry->isDir){
            int size = entry->size;

            if(entry->startDataBlock != -1){
                int currentDataBlock = entry->startDataBlock;
                int nextDataBlock = fs->FATfs->FAT[currentDataBlock];
                while(nextDataBlock != EF){
                    int temp = fs->FATfs->FAT[nextDataBlock];
                    fs->FATfs->FAT[nextDataBlock] = FREE;
                    memset(&fs->FATfs->data[nextDataBlock * BLOCK_SIZE], '\0', BLOCK_SIZE);
                    nextDataBlock = temp;
                }
                fs->FATfs->FAT[currentDataBlock] = FREE;
                memset(&fs->FATfs->data[currentDataBlock * BLOCK_SIZE], '\0', BLOCK_SIZE);
            }

            fs->FATfs->FAT[prevBlock] = fs->FATfs->FAT[entryBlock];
            fs->FATfs->FAT[entryBlock] = FREE;

            memset(&fs->FATfs->data[entryBlock * BLOCK_SIZE], '\0', BLOCK_SIZE);

            fs->currentDir->numEntries--;
            fs->currentDir->size -= size;
            printf("File '%s' erased. \n", name);
            return;
        }

        prevBlock = entryBlock;
        entryBlock = fs->FATfs->FAT[entryBlock];
    }

    printf("Error: File '%s' not found. \n", name);
}

void eraseFileData(FileSystem *fs, const char * name){
    DirEntry* currentDir = fs->currentDir;
    int entryBlock = currentDir->dirBlock;
    int prevBlock = currentDir->dirBlock;

    while(entryBlock!=EF){
        DirEntry* entry = (DirEntry*)(&fs->FATfs->data[entryBlock * BLOCK_SIZE]);
        if(strcmp(entry->name, name) == 0 && !entry->isDir){
            int size = entry->size;
            int fileDataSize = size - BLOCK_SIZE;

            if(entry->startDataBlock != -1){
                int currentDataBlock = entry->startDataBlock;
                int nextDataBlock = fs->FATfs->FAT[currentDataBlock];
                while(nextDataBlock != EF){
                    int temp = fs->FATfs->FAT[nextDataBlock];
                    fs->FATfs->FAT[nextDataBlock] = FREE;
                    memset(&fs->FATfs->data[nextDataBlock * BLOCK_SIZE], '\0', BLOCK_SIZE);
                    nextDataBlock = temp;
                }
                fs->FATfs->FAT[currentDataBlock] = FREE;
                memset(&fs->FATfs->data[currentDataBlock * BLOCK_SIZE], '\0', BLOCK_SIZE);
            }
            else{
                printf("Error: File '%s' has no data. \n", name);
                return;
            }

            entry->size = BLOCK_SIZE;
            entry->startDataBlock = -1;
            fs->currentDir->size -= fileDataSize;
            printf("File '%s' datas erased. \n", name);
            return;
        }

        prevBlock = entryBlock;
        entryBlock = fs->FATfs->FAT[entryBlock];
    }

    printf("Error: File '%s' not found. \n", name);
}

void writeFile(FileSystem* fs, FileHandle *fh, const char *buf, int len){

    if(fh==NULL){
        printf("Error: Invalid file. \n");
        return;
    }

    int currentBlock = fh->currentDataBlock;
    int fileOffset = fh->pos % BLOCK_SIZE;
    int bytesToWrite = len;
    int bufOffset = 0;

    if(len > 0){
        if(currentBlock == -1){
            int newBlock = findFree(fs->FATfs);
            if(newBlock == -1){
                printf("Error: no more free blocks. \n");
                return;
            }

            memcpy(&fs->FATfs->data[fh->currentFatBlock * BLOCK_SIZE] + 16, &newBlock, 4);
            fh->currentDataBlock = newBlock;

            currentBlock = newBlock;
            fs->FATfs->FAT[currentBlock] = EF;
        }
    }
    else{
        printf("Error: len must be greater than 0 \n");
        return;
    }

    if(fileOffset == 0 && fh->pos != 0){
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
        }

    while(bytesToWrite > 0){

        int inBlockWritableBytes = BLOCK_SIZE - fileOffset;
        if(inBlockWritableBytes > bytesToWrite){
            inBlockWritableBytes = bytesToWrite;
        }

        memcpy(&fs->FATfs->data[currentBlock * BLOCK_SIZE + fileOffset], &buf[bufOffset], inBlockWritableBytes);

        bytesToWrite -= inBlockWritableBytes;
        bufOffset += inBlockWritableBytes;
        fileOffset = (fileOffset + inBlockWritableBytes) % BLOCK_SIZE;

        if(bytesToWrite > 0){
            if(fs->FATfs->FAT[currentBlock] == EF){
                int newBlock = findFree(fs->FATfs);
                if(newBlock == -1){
                    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->currentFatBlock * BLOCK_SIZE]);
                    DirEntry* currentDir = (DirEntry*)(&fs->FATfs->data[file->parentDirBlock * BLOCK_SIZE]);
                    int fileDataSize = file->size - BLOCK_SIZE;
                    int finalDataSize = fh->pos + len;
                    int addedLen = 0;

                    if (finalDataSize > fileDataSize){
                        addedLen += (finalDataSize - fileDataSize);
                        file->size += addedLen;
                        currentDir->size += addedLen;
                    }

                    printf("No more free blocks, only %d bytes have been written! (%d bytes added) \n", len-bytesToWrite, addedLen);
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
    }

    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->currentFatBlock * BLOCK_SIZE]);
    DirEntry* currentDir = (DirEntry*)(&fs->FATfs->data[file->parentDirBlock * BLOCK_SIZE]);
    int fileDataSize = file->size - BLOCK_SIZE;
    int finalDataSize = fh->pos + len;
    int addedLen = 0;

    if (finalDataSize > fileDataSize){
        addedLen += (finalDataSize - fileDataSize);
        file->size += addedLen;
        currentDir->size += addedLen;
    }

    printf("File '%s' written (%d bytes added) \n", fh->name, addedLen);
}

void readFile(FileSystem* fs, FileHandle *fh, char *buf, int len){

    if(fh==NULL){
        printf("Error: Invalid file. \n");
        return;
    }

    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->currentDataBlock * BLOCK_SIZE]);
    int currentBlock = fh->currentDataBlock;
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

    DirEntry* file = (DirEntry*)(&fs->FATfs->data[fh->currentFatBlock * BLOCK_SIZE]);
    int fileDataSize = 0;
    int currentBlock = file->startDataBlock;
    int newPosBlock = newPos / BLOCK_SIZE;

    if(file->size > BLOCK_SIZE){
        fileDataSize += (file->size - BLOCK_SIZE);
    }

    if(currentBlock == -1){
        fh->pos = fileDataSize;
        printf("File '%s' seeked to position %d \n", fh->name, fileDataSize);

        return;
    }

    if(newPos < 0 || newPos > fileDataSize){
        while(fs->FATfs->FAT[currentBlock]!=EF){
            currentBlock = fs->FATfs->FAT[currentBlock];
        }

        fh->currentDataBlock = currentBlock;
        fh->pos = fileDataSize;
        printf("Error: Invalid seek position, seeked to the last position: %d, at block %d. \n", fileDataSize, currentBlock);

        return;
    }

    if(newPosBlock != 0){
        for(int i = 0; i < newPosBlock; i++){
            if(fs->FATfs->FAT[currentBlock] != EF){
                currentBlock = fs->FATfs->FAT[currentBlock];
            }
        }
    }

    fh->currentDataBlock = currentBlock;
    fh->pos = newPos;
    printf("File '%s' seeked to position %d, at block %d. \n", fh->name, newPos, currentBlock);
}

void createDir(FileSystem* fs, const char *dirname){
    DirEntry* parentDir = fs->currentDir;

    if(parentDir->numEntries >= MAX_ENTRIES){
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
    newDir->startDataBlock = freeBlock;
    newDir->size = 0;
    newDir->parentDirBlock = parentDir->startDataBlock;
    newDir->isDir = 1;
    newDir->numEntries = 0;
    //spreco di memoria? ->
    //memset(newDir->entries, FREE, sizeof(newDir->entries));

    //for max entries trova blocco libero

    parentDir->numEntries++;
    fs->FATfs->FAT[freeBlock] = EF;
}

