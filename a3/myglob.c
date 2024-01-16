#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "myglob.h"

struct filematch *head = NULL;

void freemyglob(struct filematch *p){
    struct filematch *tmp = NULL;

    while ((tmp = head) != NULL)
    {
        head = head -> next;
        free(tmp);
    }

}

int questionmatch(char *filename, char *pat, int len){
    int i = 0;
    for (i=0; i<len; i++){
        if ((filename[i] != pat[i]) && (pat[i] != '?')){
            return (0);
        }
    }
    return (1);
}

void insert(char *filename)
{
    struct filematch *new, *curr;

    /* create the new filematch */
    new = (struct filematch *) malloc(sizeof(struct filematch));

    if (new == NULL){
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
    new->filename = filename;
 
    if (head == NULL){
        head = new;
    }else{
        curr = head;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new;
    }

}

struct filematch * myglob(char *pat){
    struct dirent *dp;
    DIR *dr;
    int index;
    char *filename, *e;
    if ((dr = opendir (".")) == NULL) {
        perror ("Cannot open current directory.");
        return NULL;
    }

    /* NO asterisk*/

    e = strchr(pat, '*');
    if (e == NULL){
        while ((dp = readdir(dr)) != NULL){
            if (strlen(dp->d_name) == strlen(pat)){
                if (questionmatch(dp->d_name, pat, strlen(pat))){
                    insert(dp->d_name);
                }
            }
        }
    }
    /* 1st asterisk is in <index> */
    else{
        index = (int)(e-pat);
        while ((dp = readdir(dr)) != NULL){
            filename = dp->d_name;
            if (strlen(filename) >= (strlen(pat)-1)){
                int right = strlen(pat)-1-index;
                if (questionmatch(filename, pat, index) && 
                    questionmatch(filename+strlen(filename)-right, pat+index+1, right)){
                    insert(dp->d_name);
                }
            }
        }
    }
    closedir(dr);
    return head;
}




