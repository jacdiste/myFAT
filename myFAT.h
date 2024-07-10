#ifndef MYFAT_H
#define MYFAT_H

#define MAX_NAME_LENGHT 256
#define MAX_ENTRIES 64 //SERVE??
#define BLOCKS_NUMBER 1024
#define BLOCK_SIZE 4096
#define FREE -1
#define EF -2

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
    DirEntry* currentDir;
    int pos;
}FileHandle;


typedef struct{
    int disk[BLOCKS_NUMBER];
    char* data;
} FAT;

typedef struct{
    FAT* fat;
    char* data;
} FileSystem;


//int countFiles(DirEntry* dir);
//int countDirectories(DirEntry* dir);

FileHandle* createFile(FileSystem* fs, char *filename);
void eraseFile(FileSystem* fs, FileHandle* fh);
void writeFile(FileSystem* fs, FileHandle *fh, const void *buf, int size);
void readFile(FileSystem* fs, FileHandle *fh, void *buf, int size);
void seekFile(FileSystem* fs, FileHandle *fh, int pos);
void createDir(FileSystem* fs, const char *dirname);
void eraseDir(FileSystem* fs, const char *dirname);
void changeDir(FileSystem* fs, const char *dirname);
void listDir(FileSystem* fs);

#endif