#include <stdio.h>
#include "myglob.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: testmyglob pat\n");
        return(1);
    }

    struct filematch *p = myglob(argv[1]);
    for (; p; p = p->next)
        printf("%s\n", p->filename);
    return(0);
}