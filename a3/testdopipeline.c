
#include <stdio.h>
#include <unistd.h>
#include "dopipeline.h"
#include "parse.h"

int main()
{
    char buf[1000];
    while (printf("$ "), fgets(buf, sizeof buf, stdin)) {
        struct parseval *p = parse(buf);
        if (!p) {
            printf("%s\n", parse_error);
        } else {
            dopipeline(p->leftside, p->rightsideargv, p->outputfile);
            freeparse(p);
        }
    }
    printf("Thanks for playing!  Your final score is 0.\n");
    return(0);
}
