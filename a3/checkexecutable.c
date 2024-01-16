/* Check whether a file is executable by owner */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    int status = 0;
    if (argc < 2) {
        fprintf(stderr, "usage: %s file ...\n", argv[0]);
        return(1);
    }
    for (argc--, argv++; argc > 0; argc--, argv++) {
        struct stat statbuf;
        if (stat(*argv, &statbuf)) {
            perror(*argv);
            status = 1;
        } else {
            if (S_ISREG(statbuf.st_mode) && (statbuf.st_mode & 0100))  
            /* warning: Those extra parentheses are needed in some contexts*/
                printf("%s is executable\n", *argv);
            else
                printf("%s is not executable\n", *argv);
        }
    }
    return(status);
}