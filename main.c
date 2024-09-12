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
    createFile(fs, "e");
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

    //FileHandle* d_fh = openFile(fs, "d");
    //FileHandle* a_fh = openFile(fs, "a");

    //writeFile(fs, d_fh, "In pitch dark I go walking in your landscape Broken branches Trip me as I speak Just 'cause you feel it Doesn't mean it's there Just 'cause you feel it Doesn't mean it's there There's always a siren Singing you to shipwreck (Don't reach out, don't reach out Don't reach out, don't reach out) Steer away from theserocks We'd be a walking disaster (Don't reach out, don't reach out Don't reach out, don't reach out) Just 'cause you feel it Doesn't mean it's there (Someone on your shoulder Someone on your shoulder) Just 'cause you feel it Doesn't mean it's there (Someone on your shoulder Someone on your shoulder) There there! Why so green and lonely? Lonely, lonely?", 667); 
    //writeFile(fs, a_fh, "Testing", 8);
    //writeFile(fs, d_fh, "Aggiungo questa frase vediamo che succede", 42);

    //eraseFile(fs, "d");
    //eraseFile(fs, "a");

    char* d_buf1 = (char*)malloc(5);
    //readFile(fs, d_fh, d_buf1, 5);

    //printf("\nd_buf:");
    //while(*d_buf1){
    //    printf("%c", *d_buf1);
    //    d_buf1++;
    //}
    //printf("\n");

    //closeFile(fs, d_fh);
    //closeFile(fs, a_fh);

    free(d_buf1);
    unloadFS(fs);
    return 0;
}
