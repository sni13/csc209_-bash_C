#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include "test.h"

int pipefd[2], in, out;

// void execmd(char **cmd){

//     int x = fork ();

//     if (x == -1) {
//         perror("fork");
//         exit(1);
//     }else if (x == 0){
//         execvp(cmd[0], cmd);
//     }else{
//         int status;
//         wait(&status);
//     }

// }

void dochild(struct commandlist *leftside){
    struct commandlist *curr;
    char **cmd;

    close(pipefd[0]);  /* the reading side of the pipe */
    dup2(pipefd[1], 1);  /* automatically closes previous fd 1 */

    
    curr = leftside;
    cmd = curr->argv;
    execvp(cmd[0], cmd);
    close(pipefd[1]);  /* cleanup */

    // printf("stamp");

    // while (curr != NULL){
    //     printf("in: %d out: %d cmd: %s\n", in, out, curr->argv[0]);
    //     execmd(curr->argv);
    //     curr = curr->next;
    // }

}



void doparent(char **cmd, char *outputfile){
    wait(NULL);

    close(pipefd[1]);/* close the write side of the pipe */
    dup2(pipefd[0], 0);  /* automatically closes previous fd 0 */


    if (outputfile) {
        if ((open(outputfile, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
            perror("file");
            exit(1);
        }
    } 
    execvp(cmd[0], cmd);
    close(pipefd[0]);  /* cleanup */

}

void dopipeline(struct commandlist *leftside, char **rightsideargv, char *outputfile)
{
    int pid;

    fflush(stdout);  

    if (pipe(pipefd)) {
        perror("pipe");
        exit(1);
    }

    switch ((pid = fork())) {
    case -1:
        perror("fork");
        break;
    case 0:
        /* child */
        dochild(leftside);
        break;
    default:
        /* parent; */
        doparent(rightsideargv, outputfile);
    }

}