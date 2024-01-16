/*
 * Parse commands of the form
 *        (cmd1; cmd2; cmd3) | cmd >file
 * , only.  If there is only one command on the left side, the parentheses
 * may be absent.  The output file redirection is optional.  Other than that,
 * it has to be exactly of this form, because that's all that assignment
 * three's dopipeline() can handle.
 *
 * If there is a parse error, the variable parse_error contains a suitable
 * string for output to the user.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dopipeline.h"
#include "parse.h"

char *parse_error;

#define MAXARGV 100

static char *input;

static enum token { EOS, LPAREN, RPAREN, SEMI, BAR, GREATER, WORD, ERROR } t;
static char *word;

static void consume();
static char **parseargv();

static void freecmdlist(struct commandlist *p);
static void freeargv(char **p);
static void *emalloc(size_t size);
static char *estrdup(char *s);


struct parseval *parse(char *s)
{
    struct parseval *retval = emalloc(sizeof(struct parseval));
    retval->leftside = NULL;
    retval->rightsideargv = NULL;
    retval->outputfile = NULL;
    struct commandlist **nextp = &retval->leftside;

    input = s;
    consume();  /* prime the token buffer */

    int sawparen = 0;  /* 1 for saw lparen, 2 for saw rparen */
    while (t != EOS && t != BAR) {
        if (sawparen == 2) {
            parse_error = "you may not have anything between the right parenthesis and the pipe";
            freeparse(retval);
            return(NULL);
        }
        switch ((int)t) {  /* (cast prevents warning about missing cases) */
        case LPAREN:
            if (sawparen) {
                parse_error = "multiple left parentheses";
                freeparse(retval);
                return(NULL);
            }
            sawparen++;
            consume();
            break;
        case RPAREN:
            if (sawparen != 1) {
                parse_error = "mismatched parentheses";
                freeparse(retval);
                return(NULL);
            }
            sawparen++;
            consume();
            break;
        case SEMI:
            if (sawparen != 1) {
                parse_error = "semicolon outside parentheses";
                freeparse(retval);
                return(NULL);
            }
            consume();
            break;
        case GREATER:
            parse_error = "attempt to output-redirect the left side of the pipe to a file";
            freeparse(retval);
            return(NULL);
        case WORD:
            *nextp = emalloc(sizeof(struct commandlist *));
            (*nextp)->argv = parseargv();
            (*nextp)->next = NULL;
            nextp = &(*nextp)->next;
            break;
        case ERROR:
            parse_error = "invalid symbol";
            freeparse(retval);
            return(NULL);
        }
    }
    if (sawparen == 1) {
        parse_error = "mismatched parentheses";
        freeparse(retval);
        return(NULL);
    }

    if (t != BAR) {
        parse_error = "missing pipe symbol";
        freeparse(retval);
        return(NULL);
    }
    consume();

    if (t != WORD) {
        parse_error = "missing command for the right side of the pipe";
        freeparse(retval);
        return(NULL);
    }
    retval->rightsideargv = parseargv();

    if (t == GREATER) {
        consume();
        if (t != WORD) {
            parse_error = "missing file name for output redirect";
            freeparse(retval);
            return(NULL);
        }
        retval->outputfile = word;
        consume();
    }

    if (t != EOS) {
        parse_error = "excess tokens";
        freeparse(retval);
        return(NULL);
    }

    return(retval);
}


void printparse(struct parseval *p)
{
    struct commandlist *cmd;
    char **argvp;

    if (p == NULL) {
        printf("[empty]\n");
        return;
    }

    printf("(");
    for (cmd = p->leftside; cmd; cmd = cmd->next) {
        for (argvp = cmd->argv; *argvp; argvp++) {
            printf("%s", *argvp);
            if (argvp[1])
                printf(" ");
        }
        if (cmd->next)
            printf("; ");
    }

    printf(") | ");

    for (argvp = p->rightsideargv; *argvp; argvp++) {
        printf("%s", *argvp);
        if (argvp[1])
            printf(" ");
    }

    if (p->outputfile)
        printf(" >%s", p->outputfile);

    printf("\n");
}


static void consume()
{
    static char mychars[] = "();|> \t\n\r\f\v";
    char *p;

    while (*input && isspace(*input))
        input++;

    if (*input == '\0') {
        t = EOS;
        return;
    }
    if ((p = strchr(mychars, *input))) {
        t = p - mychars + 1;
        input++;
        return;
    }

    /* otherwise, it's a word! */
    int wordsize = strcspn(input, mychars);
    if (wordsize == 0) {
        /*
         * I'm not sure when this would happen... requires more thought
         * which I don't feel like right now
         */
        t = ERROR;
        return;
    }
    word = emalloc(wordsize + 1);
    strncpy(word, input, wordsize);
    word[wordsize] = '\0';
    input += wordsize;
    t = WORD;
}



void freeparse(struct parseval *p)
{
    if (p) {
        freecmdlist(p->leftside);
        freeargv(p->rightsideargv);
        if (p->outputfile)
            free(p->outputfile);
        free(p);
    }
}


static void freecmdlist(struct commandlist *p)
{
    if (p) {
        freeargv(p->argv);
        freecmdlist(p->next);
        free(p);
    }
}


static void freeargv(char **p)
{
    if (p)
        for (; *p; p++)
            free(*p);
}

static void *emalloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL) {
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
    return(p);
}


static char *estrdup(char *s)
{
    /*
     * I'm not sure if I should be calling emalloc() and doing the strdup
     * myself, rather than basically copying and pasting emalloc() here.
     */
    char *p = strdup(s);
    if (p == NULL) {
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
    return(p);
}
