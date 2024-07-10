#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "myFAT.h"

FileHandle* createFile(FATFileSystem* fs, char* filename){

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
}

void writeFile(FATFileSystem* fs, FileHandle *fh, const void *buf, int size){
    if(strcmp(fs->currentDir->name,fh->currentDir->name) != 0){
        printf("Error: The file is not in this directory, use 'changeDir' to navigate.\n");
        return;
    }

    
}

void eraseFile(FATFileSystem* fs, FileHandle* fh){

}
