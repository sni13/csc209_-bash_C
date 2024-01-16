#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include "dopipeline.h"




// void spawn_proc (int in, int out, char** cmd) {
//     int x = fork ();

//     if (x == -1) {
//         perror("fork");
//         exit(1);
//     }else if (x == 0){
//         if (in != 0)
//             dup2 (in, 0);
//             close (in);
//         if (out != 1)
//             dup2 (out, 1);
//             close (out);
//         execvp(cmd[0], cmd);
//     }else{
//         int status;
//         wait(&status);
//     }
// }

void spawn(char** cmd) {

    printf("cmd: %s\n", cmd[0]);
    int x = fork ();

    if (x == -1) {
        perror("fork");
        exit(1);
    }else if (x == 0){
        execvp(cmd[0], cmd);
    }else{
        int status;
        wait(&status);
    }
}

void spawn_proc (int in, int out, char** cmd) {

    int x = fork ();

    printf("in: %d out: %d cmd: %s\n", in, out, cmd[0]);

    if (x == -1) {
        perror("fork");
        exit(1);
    }else if (x == 0){
        if (in != 0)
            dup2 (in, 0);
            close (in);
        if (out != 1)
            dup2 (out, 1);
            close (out);
        execvp(cmd[0], cmd);
    }else{
        int status;
        wait(&status);
    }
}

void doit(int in, int out, struct commandlist *leftside){

    struct commandlist *curr;
    curr = leftside;

    while (curr != NULL){
        printf("in: %d out: %d cmd: %s\n", in, out, curr->argv[0]);
        spawn(curr->argv);
        curr = curr->next;
    }

    
}

void dopipeline(struct commandlist *leftside, char **rightsideargv, char *outputfile)
{
    int pid, status;
    int in, out, pipefd[2];
    in = 0;
    out = pipefd[1];

    if (pipe(pipefd)) {
        perror("pipe");
        exit(1);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(1);
    case 0:
        /* child */
        dup2 (out, 1);
        close (out);
        doit(in, pipefd[1], leftside);
        close(pipefd[1]);
        in = pipefd[0];

    default:
        /* parent */

        pid = wait(&status);

        if (outputfile) {
            if ((out = open("file", O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
                perror("file");
                exit(1);
            }
        } else{
            out = STDOUT_FILENO;
        }

        if (in != 0)
            dup2(in, 0);
 
        spawn_proc(in, out, rightsideargv);

    }
}