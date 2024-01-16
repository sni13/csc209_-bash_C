struct filematch {
    char *filename;
    struct filematch *next;
};


extern struct filematch * myglob(char *pat);
extern void freemyglob(struct filematch *p);

// extern void insert(char *filename);
// extern int questionmatch(char *filename, char *pat, int len);

