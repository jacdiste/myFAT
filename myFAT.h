#ifndef MYFAT_H
#define MYFAT_H

#define MAX_NAME_LENGHT 256
#define MAX_ENTRIES 64 //SERVE??
#define BLOCKS_NUMBER 1024
#define BLOCK_SIZE 4096
#define FREE 0

typedef struct DirEntry{
    char* name;
    int startBlock;
    int size;
    int parentDirBlock;

    int isDir;
    int numFiles;
    int numDir;
    struct DirEntry** entries;
} DirEntry;

typedef struct{
    int pos;
}FileHandle;

typedef struct{
    int disk[BLOCKS_NUMBER];
    DirEntry* currentDir;
    char* data;
} FATFileSystem;


//int countFiles(DirEntry* dir);
//int countDirectories(DirEntry* dir);

FileHandle* createFile(FATFileSystem* fs, char *filename);
void eraseFile(FATFileSystem* fs, FileHandle* fh);
void writeFile(FATFileSystem* fs, FileHandle *fh, const void *buf, int size);
void readFile(FATFileSystem* fs, FileHandle *fh, void *buf, int size);
void seekFile(FATFileSystem* fs, FileHandle *fh, int pos);
void createDir(FATFileSystem* fs, const char *dirname);
void eraseDir(FATFileSystem* fs, const char *dirname);
void changeDir(FATFileSystem* fs, const char *dirname);
void listDir(FATFileSystem* fs);

#endif