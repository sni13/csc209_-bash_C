#include <stdio.h>
#include <stdlib.h>

/* Run with appropriate PATH settings, e.g. "PATH=:/bin ./a.out" */

int main()
{
    int i;
    static char *cmds[] = { "cat", "./cat", "du", "pine", "frob", "/usr/local/bin/python" };
    //static char *cmds[] = { "cat", "du", "pine", "frob"};
    //static char *cmds[] = {"./testmyglob", "/Users/nisailing/desktop/CSC209_Summer!/a2/findnonascii"};
    extern char *searchpath(char *cmd);

    for (i = 0; i < sizeof cmds / sizeof cmds[0]; i++)
        /*
         * We're making use of the fact that this C library's printf() of a
         * null string outputs a helpful debugging string -- wouldn't do this
         * in production code, but it's good for an interactive test
         */
        printf("searchpath(%s) returns %s\n", cmds[i], searchpath(cmds[i]));

    return(0);
}
