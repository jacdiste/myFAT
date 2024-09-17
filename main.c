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
    printf("\n--- myFAT Menu ---\n");
    printf("Commands:\n");
    printf("1. createFile <filename>\n");
    printf("2. eraseFile <filename>\n");
    printf("3. writeFile <filename> <text> <length>\n");
    printf("4. readFile <filename> <length>\n");
    printf("5. createDir <dirname>\n");
    printf("6. eraseDir <dirname>\n");
    printf("7. listDir <dirname>\n");
    printf("8. quit\n");
    printf("----------------------------------------\n");
}

int main(int argc, char *argv[]) {

    FileSystem* fs = loadFS("myFATfs");
    
    //printMenu();

    createFile(fs, "a");
    //createFile(fs, "a");
    createDir(fs, "1");
    createFile(fs, "b");
    listDir(fs);
    changeDir(fs, "1");
    createFile(fs, "1a");
    createDir(fs, "2");
    listDir(fs);
    changeDir(fs, "2"); //NON VA
    createFile(fs, "2a");
    listDir(fs);
    changeDir(fs, "..");
    changeDir(fs, "..");
    //createFile(fs, "e");
    //createFile(fs, "f");
    //createFile(fs, "g");
    //createFile(fs, "h");
    //createFile(fs, "i");

    //eraseFile(fs, "a");
    //eraseFile(fs, "a");
    //changeDir(fs, "subroot");
    //createFile(fs, "a");
    //eraseFile(fs, "b");
    //eraseFile(fs, "c");
    //eraseFile(fs, "d");
    //eraseFile(fs, "h");
    //eraseFile(fs, "i");

    FileHandle* a_fh = openFile(fs, "a");
    //FileHandle* a_fh = openFile(fs, "a");
    //FileHandle* b_fh = openFile(fs, "b");
    //FileHandle* c_fh = openFile(fs, "c");
    //FileHandle* d_fh = openFile(fs, "d");
    //seekFile(fs, d_fh, 1000);
//
    //writeFile(fs, a_fh, "In pitch dark I go walking in your landscape Broken branches Trip me as I speak Just 'cause you feel it Doesn't mean it's there Just 'cause you feel it Doesn't mean it's there There's always a siren Singing you to shipwreck (Don't reach out, don't reach out Don't reach out, don't reach out) Steer away from theserocks We'd be a walking disaster (Don't reach out, don't reach out Don't reach out, don't reach out) Just 'cause you feel it Doesn't mean it's there (Someone on your shoulder Someone on your shoulder) Just 'cause you feel it Doesn't mean it's there (Someone on your shoulder Someone on your shoulder) There there! Why so green and lonely? Lonely, lonely?", 666); 
    //writeFile(fs, b_fh, "To where all you hear is water sounds, Lush as the wind upon a tree.", 68);
    writeFile(fs, a_fh, "Prova", 5);
    //writeFile(fs, a_fh, "To where all you hear is water sounds, Lush as the wind upon a tree.", 68);
    //writeFile(fs, a_fh, "Prova", 5);
    //writeFile(fs, a_fh, "fwefwfwefiwuhcownpwinwiwoiefh0weAAAAAAAA", 40);
    //writeFile(fs, d_fh, "Aggiungo questa frase vediamo che succede", 41);

    //eraseFileData(fs, "a");
    //eraseFile(fs, "d");
    //eraseFile(fs, "a");
    //seekFile(fs, a_fh, 0);
    //seekFile(fs, a_fh, 0);

    //char a_buf1[1000] = {0};
    //readFile(fs, a_fh, a_buf1, 10);
    //printf("Read: %s\n", a_buf1);
    //char a_buf1[1000] = {0};
    //readFile(fs, a_fh, a_buf1, 10);
    //printf("Read: %s\n", a_buf1);

    closeFile(fs, a_fh);
    //closeFile(fs, a_fh);
    //closeFile(fs, b_fh);
    //closeFile(fs, c_fh);
    //closeFile(fs, d_fh);
    //createDir(fs, "subroot");

    unloadFS(fs);
    return 0;
}
