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

    FATFileSystem* fs = (FATFileSystem*) malloc(sizeof(FATFileSystem));
    memset(fs->disk, FREE, sizeof(fs->disk));
    int fd = open("filesystem", O_RDWR | O_CREAT, 0666);
    ftruncate(fd, BLOCK_SIZE*BLOCKS_NUMBER);
    fs->data = mmap(NULL, BLOCK_SIZE*BLOCKS_NUMBER, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memset(fs->data, '\0', BLOCK_SIZE*BLOCKS_NUMBER);
    close(fd);

    /* STAMPO DISK E DATA */
    //int i = 0;
    //for(i; i < BLOCKS_NUMBER; i++) {
    //    printf("%d", fs->disk[i]);
    //}

    //printf("\n%d", i);

    //int j = 0;
    //for(j; j < BLOCK_SIZE*BLOCKS_NUMBER; j++) {
    //    printf("%d", fs->data[j]);
    //}

    //printf("\n%d", j);

    munmap(fs->data, BLOCKS_NUMBER*BLOCK_SIZE);
    free(fs);
}
