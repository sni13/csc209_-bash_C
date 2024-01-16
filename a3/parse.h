struct parseval {
    struct commandlist *leftside;
    char **rightsideargv;
    char *outputfile;
};

extern struct parseval *parse(char *s);
extern void printparse(struct parseval *p);
extern void freeparse(struct parseval *p);
extern char *parse_error;