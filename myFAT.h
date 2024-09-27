#ifndef MYFAT_H
#define MYFAT_H

#define MAX_NAME_LENGHT 16
#define MAX_ENTRIES 32
#define BLOCKS_NUMBER 1024
#define BLOCK_SIZE 40
#define MAX_DATA 40880
#define FREE -1
#define EF -2

typedef struct DirEntry{
    char name[MAX_NAME_LENGHT];
    int outerBlockIndex;
    int size;
    int currentBlockIndex;
    int parentDirBlockIndex;
    int isDir;
    int numEntries;
} DirEntry;

typedef struct{
    char parentDirName[MAX_NAME_LENGHT];
    char name[MAX_NAME_LENGHT];
    int currentDataBlockIndex;
    int currentFatBlockIndex;
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
void adjustSize(FileSystem* fs, DirEntry* parentDir, int targetSize, int type);
FileHandle* openFile(FileSystem* fs, const char* name);
void closeFile(FileSystem* fs, FileHandle* fh);
void eraseFileData(FileSystem* fs, const char* name);
void printFs(FileSystem* fs, DirEntry* dir, int level);

// Requested functions
void createFile(FileSystem* fs, const char* name);
void eraseFile(FileSystem* fs, const char* name, DirEntry* parentDir);
void writeFile(FileSystem* fs, FileHandle *fh, const char *buf, int len);
void readFile(FileSystem* fs, FileHandle *fh, char *buf, int len);
void seekFile(FileSystem* fs, FileHandle *fh, int newPos);
void createDir(FileSystem* fs, const char *dirName);
void eraseDir(FileSystem* fs, const char *dirName, DirEntry* parentDir);
void changeDir(FileSystem* fs, const char *dirName);
void listDir(FileSystem* fs);


#endif