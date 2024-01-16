struct commandlist {
    char **argv;
    struct commandlist *next;
};

extern void dopipeline(struct commandlist *leftside, char **rightsideargv,
                         char *outputfile);