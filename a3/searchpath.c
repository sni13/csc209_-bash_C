#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define LENGTH 1000

int ifExecutable(char *path){
	struct stat statbuf;

	//printf("path: <%s>\n", path);
	
    if (stat(path, &statbuf)) {
        //printf("stat() error\n");
        return (0);
    } else {
        if (S_ISREG(statbuf.st_mode) && (statbuf.st_mode & 0100)){
            //printf("%s is executable\n", path);
            return (1);
        }
        else{
            //printf("%s is not executable\n", path);
            return (0);
        }
    }
}

char *searchpath(char *cmd){

    static char path[LENGTH];
    static char tmp[LENGTH];
    const char s[2] = ":";
    const char ch = '/';
    char *token, *e;
    int i;

    if (strlen(getenv("PATH")) > LENGTH){
        perror("PATH length exceeds limit\n");
        exit(1);
    }

    strcpy(path, getenv("PATH"));

    /* case 1: contains a slash */
    if ((e = strchr(cmd, ch))!= NULL){

        i = ifExecutable(cmd);
        //printf("case 1 if executable: %d\n", i);
        if (i == 1){
            return cmd;
        }
        else{
            return NULL;
        }
    }
    
    /* case 2: does not contain a slash*/
    token = strtok(path, s);

    while (token != NULL){
        //printf("Current Token: %s\n", token);
        if (strcmp(token, "") == 0 || strcmp(token, ".") == 0){
            strcpy(tmp, ".");
        }else{
            strcpy(tmp, token);}

        strcat(tmp, "/");
        strncat(tmp, cmd, LENGTH - 2 -1);
        //printf("%s\n", tmp);
        i = ifExecutable(tmp);
        //printf("case 2 if executable: %d\n", i);
        if (i == 1){
            return tmp;
        }
        token = strtok(NULL, s);
    }
    return NULL;
}
