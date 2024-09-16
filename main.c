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
    //createFile(fs, "e");
    //createFile(fs, "f");
    //createFile(fs, "g");
    //createFile(fs, "h");
    //createFile(fs, "i");

    //eraseFile(fs, "a");
    //eraseFile(fs, "b");
    //eraseFile(fs, "c");
    //eraseFile(fs, "d");
    //eraseFile(fs, "e");
    //eraseFile(fs, "f");
    //eraseFile(fs, "g");
    //eraseFile(fs, "h");
    //eraseFile(fs, "i");

    FileHandle* d_fh = openFile(fs, "d");
    FileHandle* a_fh = openFile(fs, "a");
    FileHandle* b_fh = openFile(fs, "b");
    FileHandle* c_fh = openFile(fs, "c");
    //printf("\n%d\n", a_fh->pos);

    //seekFile(fs, a_fh, 1000);
    //seekFile(fs, b_fh, 1000);
    //seekFile(fs, c_fh, 1000);
    //seekFile(fs, d_fh, 1000);
//
    writeFile(fs, a_fh, "In pitch dark I go walking in your landscape Broken branches Trip me as I speak Just 'cause you feel it Doesn't mean it's there Just 'cause you feel it Doesn't mean it's there There's always a siren Singing you to shipwreck (Don't reach out, don't reach out Don't reach out, don't reach out) Steer away from theserocks We'd be a walking disaster (Don't reach out, don't reach out Don't reach out, don't reach out) Just 'cause you feel it Doesn't mean it's there (Someone on your shoulder Someone on your shoulder) Just 'cause you feel it Doesn't mean it's there (Someone on your shoulder Someone on your shoulder) There there! Why so green and lonely? Lonely, lonely?", 666); 
    //writeFile(fs, b_fh, "To where all you hear is water sounds, Lush as the wind upon a tree.", 68);
    //writeFile(fs, c_fh, "Prova", 5);
    //writeFile(fs, a_fh, "fwefwfwefiwuhcownpwinwiwoiefh0weAAAAAAAA", 40);
    //writeFile(fs, d_fh, "Aggiungo questa frase vediamo che succede", 41);

    //eraseFileData(fs, "a");
    //eraseFile(fs, "d");
    //eraseFile(fs, "a");

    //char* d_buf1 = (char*)malloc(5);
    //readFile(fs, d_fh, d_buf1, 5);

    //printf("\nd_buf:");
    //while(*d_buf1){
    //    printf("%c", *d_buf1);
    //    d_buf1++;
    //}
    //printf("\n");

    closeFile(fs, d_fh);
    closeFile(fs, a_fh);
    closeFile(fs, b_fh);
    closeFile(fs, c_fh);

    //free(d_buf1);
    unloadFS(fs);
    return 0;
}
