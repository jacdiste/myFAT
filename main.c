#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "myFAT.h"

int main(int argc, char *argv[]) {

    FileSystem* fs = loadFS("myFATfs");
    
    createFile(fs, "a");
    createFile(fs, "b");
    createFile(fs, "c");
    createFile(fs, "d");

    for(int i = 0; i<100; i++){
        printf("Block %d, Address %p: %d\n", i, &fs->FATfs->FAT[i], fs->FATfs->FAT[i]);
    }

    for(int i = 0; i<100; i++){
        printf("Block %d, Address %p: %d\n", i, &fs->FATfs->data[i], fs->FATfs->data[i]);
    }

    eraseFile(fs, "a");
    eraseFile(fs, "b");
    eraseFile(fs, "c");

    for(int i = 0; i<100; i++){
        printf("Block %d, Address %p: %d\n", i, &fs->FATfs->FAT[i], fs->FATfs->FAT[i]);
    }

    for(int i = 0; i<100; i++){
        printf("Block %d, Address %p: %d\n", i, &fs->FATfs->data[i], fs->FATfs->data[i]);
    }

    FileHandle* d_fh = openFile(fs, "d");
    FileHandle* a_fh = openFile(fs, "a");

    closeFile(fs, d_fh);
    closeFile(fs, a_fh);
    
    unloadFS(fs);
    return 0;
}
