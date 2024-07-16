#ifndef MYFAT_H
#define MYFAT_H

#define MAX_NAME_LENGHT 256
#define MAX_ENTRIES 64
#define BLOCKS_NUMBER 1024
#define BLOCK_SIZE 64
#define FREE -1
#define EF -2

typedef struct DirEntry{
    char name[MAX_NAME_LENGHT];
    int startBlock;
    int size;
    int parentDirBlock;

    int isDir;
    int numFiles;
    int numDir;
    int entries[MAX_ENTRIES];
} DirEntry;

typedef struct{
    DirEntry* currentDir;
    int startBlock;
    int pos;
} FileHandle;

typedef struct{
    int FAT[BLOCKS_NUMBER];
    char data[BLOCK_SIZE * BLOCKS_NUMBER];
} FATFileSystem;

typedef struct{
    FATFileSystem* FATfs;
    DirEntry* currentDir;
} FileSystem;


//Helper functions
FileSystem* loadFS(const char* name);
void unloadFS(FileSystem* fs);
int findFree(FATFileSystem* FATfs);
FileHandle* openFile(FileSystem* fs, const char* name);
void closeFile(FileSystem* fs, FileHandle* fh);

// Requested functions
void createFile(FileSystem* fs, const char* name);
void eraseFile(FileSystem* fs, const char* name);
void writeFile(FileSystem* fs, FileHandle *fh, const char *buf, int len);
void readFile(FileSystem* fs, FileHandle *fh, char *buf, int len);
void seekFile(FileSystem* fs, FileHandle *fh, int pos);
void createDir(FileSystem* fs, const char *dirname);
void eraseDir(FileSystem* fs, const char *dirname);
void changeDir(FileSystem* fs, const char *dirname);
void listDir(FileSystem* fs);


#endif