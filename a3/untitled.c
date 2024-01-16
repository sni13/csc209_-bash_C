#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include "dopipeline.h"



void dopipeline(struct commandlist *leftside, char **rightsideargv, char *outputfile){
    int fd[2], file_dsptr;
    struct commandlist *curr;
    char **cmd;
    pid_t cpid;
    close(1); /* so that output file should get file descriptor 1 */

    if (outputfile != NULL) {
        if ((file_dsptr = open(outputfile, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
            perror("file");
            exit(1);
        }
    }else{
        file_dsptr = 1;
    }

    if(pipe(fd) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if ((cpid = fork()) == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if( cpid == 0 ){ 
        /*child process */
        close( fd[0] ); /* close read end of pipe */
        if( -1 == dup2(fd[1] , file_dsptr)){ /* duplicate fd[1] to fd where data.txt points */
            exit( EXIT_FAILURE );
        }
        curr = leftside;
        while (curr != NULL){
            cmd = curr->argv;
            if (-1 == execvp(cmd[0], cmd)){
                exit( EXIT_FAILURE);
            }
            curr = curr->next;
        }

        close( fd[1] );
        _exit( EXIT_SUCCESS );
    }else{
        wait( NULL ); /* wait for child to completes */
        close( fd[1] ); /* close write end of pipe */
        if( -1 == dup2(fd[0] , STDIN_FILENO)) {
                exit( EXIT_FAILURE );
        }
        if(-1 == execvp(rightsideargv[0], rightsideargv)){
                exit( EXIT_FAILURE );
        }
        close( fd[0] );
        _exit( EXIT_SUCCESS );

    }

}






