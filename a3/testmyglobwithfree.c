/*
 * In testmyglob.c, it was not necessary to free the "struct filematch"
 * data structure because we were about to exit; but this revised version
 * shows how we would call freemyglob() when needed.
 */

#include <stdio.h>
#include "myglob.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: testmyglob pat\n");
        return(1);
    }

    struct filematch *theglob = myglob(argv[1]);
    for (struct filematch *p = theglob; p; p = p->next)
        printf("%s\n", p->filename);

    freemyglob(theglob);
    return(0);
}
