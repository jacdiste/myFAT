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

    int used = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    if(!used){
        memset(FATfs->FAT, FREE, sizeof(FATfs->FAT));
        memset(FATfs->data, 0, sizeof(FATfs->data));

        DirEntry* root = (DirEntry*)(&FATfs->data[0]);
        strcpy(root->name, "/");
        root->startBlock = 0;
        root->size = 0;
        root->parentDirBlock = -1;
        root->isDir = 1;
        root->numFiles = 0;
        root->numDir = 0;
        memset(root->entries, FREE, sizeof(root->entries));

        fs->currentDir = root;
        FATfs->FAT[0] = EF;
    }

    else{
        fs->currentDir = (DirEntry*)(&FATfs->data[0]);
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

/*FileHandle* createFile(FileSystem* fs, char* filename){

    DirEntry* currentDir = fs->currentDir;

    for(int i=0; i<currentDir->numFiles; i++){
        if(strcmp(currentDir->entries[i]->name, filename) == 0){
            printf("Error: File already exists\n");
            return NULL;
        }
    }

    int found = -1;

    for(int i = 0; i<BLOCKS_NUMBER; i++){
        if(fs->disk[i] == FREE){
            found = i;
            fs->disk[found] = EF;
            break;
        }
    }

    if(found == -1){
        printf("Error: Disk is full\n");
        return NULL;
    }

    DirEntry* file = (DirEntry*)malloc(sizeof(DirEntry));
    file->name = filename;
    file->startBlock = found;
    file->size = 0;
    file->parentDirBlock = currentDir->startBlock;
    file->isDir = 0;
    
    fs->currentDir->size++;
    fs->currentDir->numFiles++;
    fs->currentDir->entries[fs->currentDir->numFiles-1] = file;

    FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
    fh->pos = 0;

    return fh;
} */