#ifndef MYFAT_H
#define MYFAT_H

#define MAX_NAME_LENGHT 256
#define MAX_ENTRIES 100
#define BLOCKS_NUMBER 1024
#define BLOCK_SIZE 4096
#define FREE -1

typedef struct{
    char name[MAX_NAME_LENGHT];
    int startBlock;
    int size;
    int isDir;
    int parentDirBlock;
} DirEntry;

typedef struct{
    int pos;
}FileHandle;

typedef struct{
    int disk[BLOCKS_NUMBER];
    DirEntry entries[MAX_ENTRIES];
    char* data;
} FATFileSystem;


void createFile(FATFileSystem* fs, const char* filename);
void eraseFile(FATFileSystem* fs, const char* filename);
void writeFile(FATFileSystem* fs, const char* filename, const char* data);
void readFile(FATFileSystem* fs, const char* filename, char* data, int size);
void seekFile(FATFileSystem* fs, const char* filename, int position);
void createDir(FATFileSystem* fs, const char* dirname);
void eraseDir(FATFileSystem* fs, const char* dirname);
void changeDir(FATFileSystem* fs, const char* dirname);
void listDir(FATFileSystem* fs);

#endif