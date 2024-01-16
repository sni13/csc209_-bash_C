#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <sys/stat.h> 

static char *progname;
static int currPos, oldPos, currSize, oldSize; 
struct stat stats;

void usage(){
    printf("usage: %s [-n] posfile logfile oldlogfile\n", progname);
}

int getSize(char *name){
    if (stat(name, &stats)) {
        perror(name);
        return -1;
    }else{
        return (long)stats.st_size;
    }
}

int ifRotated(){return (oldPos != oldSize) || (currPos > currSize);}


int outputFile(FILE *f, int start, int end){
    char ch;
    int i = start;

    if (fseek(f, start, SEEK_SET) != 0){
        return -1;
    }
    while (((ch = getc(f)) != EOF) && (i < end)){
        putchar(ch);
    }
    return 0;
}

int main (int argc, char ** argv){

    int option, new;
    progname = argv[0];
    FILE *auxFile;
    FILE *currFile;
    FILE *oldFile;

    if (argc != 4 && argc != 5){
        usage();
        return 1;}

    while ((option = getopt(argc, argv, "n")) != -1){
        switch(option)
        {
            case 'n':
                new = 1;
                break;
            default:
                new = 0;
                break;
        }
    }
    //open/create auxFile;
    //read postions from argv[optind]


    // file not exist; check if new
    if (stat(argv[optind], &stats){
        if (new == 1){
            if ((auxFile=fopen(argv[optind], "w+"))== NULL){
                perror(argv[optind]);
                return 1;
            currPos = oldPos = 0;
        else{
            perror(argv[optind]);
            return 1;
        }
    // file must exist, safely open it
    }else{
        auxFile=fopen(argv[optind], "w+");
        if (fscanf(auxFile, "%d %d", &currPos, &oldPos) != 2){
            perror(argv[optind]);
            usage();
            return 1;
        }    
    }
    


    if ((auxFile=fopen(argv[optind], "r+"))== NULL){
        // if <posFile> does not exist
        if (new == 1){
            if ((auxFile=fopen(argv[optind], "w+"))== NULL){
                perror(argv[optind]);
                return 1;}
            currPos = oldPos = 0;
        }else{
            perror(argv[optind]);
            return 1;
        }
    }else{
        if (fscanf(auxFile, "%d %d", &currPos, &oldPos) != 2){
            perror(argv[optind]);
            return 1;
        }
    }    

    // case 1: file does not exist
    if (stat(argv[optind], &stats) || new == 0){
        auxFile=fopen(argv[optind], "r");
        perror(argv[optind]);
        return 1;}

    auxFile=fopen(argv[optind], "w+");

    if (!stat(argv[optind], &stats)){
        if (fscanf(auxFile, "%d %d", &currPos, &oldPos) != 2)
        {
            perror(argv[optind]);
            return 1;
        }
    } else{
        currPos = oldPos = 0;
    }


    //printf("currPos = %d; oldPos = %d \n", currPos, oldPos);

    //open currFile, oldFile
    //find currSize and oldSize;

    if ((currFile=fopen(argv[optind+1], "r"))== NULL){
        perror(argv[optind+1]);
        return 1;
    }
    if ((oldFile=fopen(argv[optind+2], "r"))== NULL){
        perror(argv[optind+2]);
        return 1;
    }

    if ((currSize = getSize(argv[optind+1])) == -1){
        perror(argv[optind+1]);
        return 1;}
    if ((oldSize = getSize(argv[optind+2])) == -1){
        perror(argv[optind+2]);
        return 1;}

    // determine whether the logs have been rotated since last run
    // if rotated, output oldFile <from currPos to oldSize>
    // and, set <currPos = 0> onwards

    if (ifRotated()){
        if (outputFile(oldFile, currPos, oldSize) == -1){
            return 1;}
        currPos = 0;
    }

    // Either case, output currFile <from currPos to currSize>
    if (outputFile(currFile, currPos, currSize) == -1){
        return 1;
    }

    // save the updated file sizes to the auxiliary file
    fprintf(auxFile, "%d \n%d\n",currSize, oldSize);
    fclose(auxFile);
    fclose(currFile);
    fclose(oldFile);
}