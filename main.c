#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "myFAT.h"

void printMenu() {
    printf("\n-------------- myFAT Menu --------------\n");
    printf("1. Create a file\n");
    printf("2. Erase a file\n");
    printf("3. Write to a file\n");
    printf("4. Read from a file\n");
    printf("5. Position the file offset \n");
    printf("6. Create a directory\n");
    printf("7. Erase a directory\n");
    printf("8. Move to another directory\n");
    printf("9. List the current directory\n");
    printf("10. Print the FileSystem\n");
    printf("0. Quit\n");
    printf("----------------------------------------\n");
}

int main(int argc, char *argv[]) {

    FileSystem* fs = loadFS("myFATfs");

//-------------------------8<----------------------------------

    int choice;
    char name[100];
    char buffer[MAX_DATA];
    int pos;
    int len;
    int running = 1;

    while (running) {
        printMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter file name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                createFile(fs, name);
                break;

            case 2:
                printf("Do you want to erase only the file datas? [1/0] ");
                scanf("%d", &choice);
                getchar();
                printf("Enter file name to erase: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                if(choice){
                    eraseFileData(fs, name);
                } else {
                    eraseFile(fs, name, fs->currentDir);
                }
                break;

            case 3:
                printf("Enter file name to write to: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                printf("Enter the text to write: ");
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                len = strlen(buffer);
                printf("Enter the position where you want to write from: ");//
                scanf("%d", &pos);//
                getchar();//
                FileHandle* fh = openFile(fs, name);
                if(fh == NULL){
                    break;
                }
                seekFile(fs, fh, pos);//
                writeFile(fs, fh, buffer, len);
                closeFile(fs, fh);
                break;

            case 4:
                printf("Enter file name to read from: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                printf("Enter number of bytes to read: ");
                scanf("%d", &len); //se non inserisco un numero?
                getchar();
                FileHandle* fh_read = openFile(fs, name);
                if(fh_read == NULL){
                    break;
                }
                char* readBuf = (char*)malloc(MAX_DATA);
                readFile(fs, fh_read, readBuf, len);
                if(fh_read->currentDataBlockIndex != -1){
                    printf("Read data: %s\n", readBuf);
                }
                free(readBuf);
                closeFile(fs, fh_read);
                break;

            case 5:
                printf("Enter file name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                printf("Enter the position to seek from: ");
                scanf("%d", &pos);
                getchar();
                FileHandle* fh_seek = openFile(fs, name);
                if(fh_seek == NULL){
                    break;
                }
                seekFile(fs, fh_seek, pos);
                break;

            case 6:
                printf("Enter directory name to create: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                createDir(fs, name);
                break;

            case 7:
                printf("Enter directory name to erase: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                eraseDir(fs, name, fs->currentDir);
                break;

            case 8:
                printf("Enter directory where you want to move: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                changeDir(fs, name);
                break;

            case 9:
                listDir(fs);
                break;

            case 10:
                DirEntry* root = (DirEntry*)(&fs->FATfs->data[0]);
                printFs(fs, root, 0);
                break;

            case 0:
                printf("Exiting the file system...\n");
                printf("Exited successfully \n");
                running = 0;
                break;

            default:
                printf("Invalid choice. Please enter a number between 1 and 10, or 0 to quit.\n");
                break;
        }
    }

//-------------------------8<----------------------------------

    unloadFS(fs);
    return 0;
}
