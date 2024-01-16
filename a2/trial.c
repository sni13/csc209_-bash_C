#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

static char *progname;
static int currPos, oldPos;

void usage(){
    printf("usage: %s [-n] posfile logfile oldlogfile\n", progname);
}

int main (int argc, char ** argv){

    int option, new;
    progname = argv[0];
    FILE * auxFile;

    if (argc != 4 && argc != 5){
        usage();
        return 1;
    }

    while ((option = getopt(argc, argv, "n")) != -1){
        switch(option)
        {
            case 'n':
                //create posfile with content '0 0'
                new = 1;
                break;
            default:
                new = 0;
                break;
        }
    }
    printf("optind is %d\n", optind);


    //readPostions from argv[optind]
    //r+ doesn't create a new file if such file doesn't exist

    if ((auxFile=fopen(argv[optind], "r+"))== NULL){
        // if <posFile> does not exist
        // 
        printf("cannot open existing file named%s\n ", argv[optind]);
        if (new){
            printf("try creating new file named%s\n", argv[optind]);
            if ((auxFile=fopen(argv[optind], "w+"))== NULL){
                perror(argv[optind]);
                return 1;}
            currPos = oldPos = 0;
        }else{
            perror(argv[optind]);
            return 1;
        }
    }else{
        printf("opened existing file named %s\n ", argv[optind]);
        if (fscanf(auxFile, "%d %d", &currPos, &oldPos) != 2){
            perror(argv[optind]);
            return 1;
        }
    }

    printf("currPos = %d; oldPos = %d \n", currPos, oldPos);
}