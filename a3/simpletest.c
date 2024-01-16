
#include <stdio.h>
#include "test.h"

/* ls | tr e f > file */

int main()
{
    char *myargv1[] = { "ls", NULL };
    char *myargv2[] = { "tr", "e", "f", NULL };
    struct commandlist c = { myargv1, NULL };
    dopipeline(&c, myargv2, "file");
    return(0);
}