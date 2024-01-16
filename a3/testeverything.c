#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "myglob.h"
#include "dopipeline.h"
#include "parse.h"

int main()
{
    char buf[1000];
    extern void expandglob(char ***argvp);
    while (printf("$ "), fgets(buf, sizeof buf, stdin)) {
        struct parseval *p = parse(buf);
        if (!p) {
            printf("%s\n", parse_error);
        } else {
            for (struct commandlist *cp = p->leftside; cp; cp = cp->next)
                expandglob(&cp->argv);
            expandglob(&p->rightsideargv);
            dopipeline(p->leftside, p->rightsideargv, p->outputfile);
            freeparse(p);
        }
    }
    printf("Thanks for playing!  Your final score is 0.\n");
    return(0);
}


void expandglob(char ***argvp)
{
    int i, newsize, newp;
    char **ap, **newargv;
    extern int argvlen(char **argv);
    extern int filematchlen(struct filematch *p);

    /*
     * First, find all of the expansions -- it's work to do this in advance
     * and store them, but it's actually necessary because it's the only
     * way to find what to pass to malloc below, unless we want to do
     * lots of mallocs, which would be bad because malloc is slow.
     */
    struct filematch *expansion[argvlen(*argvp)];
    newsize = 0;
    for (ap = *argvp, i = 0; *ap; ap++, i++) {
        expansion[i] = myglob(*ap);
        if (expansion[i])
            newsize += filematchlen(expansion[i]);
        else
            newsize++;
    }

    /* now, the malloc and then fill it up */
    if ((newargv = malloc((newsize + 1) * sizeof(char *))) == NULL) {
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
    newp = 0;
    for (ap = *argvp, i = 0; *ap; ap++, i++) {
        if (expansion[i]) {
            free(*ap);
            struct filematch *p;
            for (p = expansion[i]; p; p = p->next) {
                newargv[newp++] = p->filename;
                p->filename = NULL;  /* so that it isn't freed below */
            }
            freemyglob(expansion[i]);
        } else {
            newargv[newp++] = *ap;
        }
    }
    newargv[newp] = NULL;

    /*
     * free the old shell of a data structure (the strings have already been
     * either freed or reused)
     */
    free(*argvp);
    *argvp = newargv;
}


int argvlen(char **argv)
{
    int size = 0;
    while (*argv++)
        size++;
    return(size);
}


int filematchlen(struct filematch *p)
{
    int size = 0;
    for (; p; p = p->next)
        size++;
    return(size);
}


