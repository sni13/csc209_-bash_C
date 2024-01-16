#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int searchFilesRecursively(char *origPath){
    char path[2000];
    struct dirent *dp;
    DIR *dr;
    struct stat stats;

    // Cannot open diretory, return 2, error
    if ((dr = opendir(origPath)) == NULL){
        perror(origPath);
        return 2;
    }
    // open current dir, loop over all dir 
    while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){ 
            if (lstat(".", &stats)==0){ 
                if !(stats.st_mode & S_IFLNK){
                    
                }
            }else{ 
                perror();
            }

        }
    }

}
