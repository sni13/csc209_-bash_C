#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>


//program exit 0 if some non-ascii
//program exit 1 if all ascii
//program exit 2

static char *progname;
static int err = 0;
static int res = 1;

int isSpecialName(char *dirName){
    int i = 0;
    while (i < strlen(dirName)){
        if (isascii(dirName[i]) == 0){
            return 1;
        }
    }
    return 0;
}


void searchFilesRecursively(char *origPath){
   	
    struct dirent *dp;
    struct stat stats;
    DIR *dr;
    
    //Cannot open diretory, return 2, error
    if ((dr = opendir(origPath)) == NULL){
    	err = 1;
        perror(origPath);
        return;
    }

    char newPath[2000];
    //open current dir, loop over all dir/files
    //info stored in dp;
    while ((dp = readdir(dr)) != NULL){
    	
    	strcpy(newPath, origPath);
	    strcat(newPath, "/");
	    strcat(newPath, dp->d_name);

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
        	printf("Now reading: %s\n", dp->d_name);
      
	        //check if current name contains non-ascii
		    if (isSpecialName(dp->d_name)== 1){
				printf("%s\n", newPath);
				res = 0;
		    }

		    //call lstat() on newPath to gather info
		    //lstat() returns 0 if success

		   
		    if (lstat(newPath, &stats)){
				if ((S_ISDIR(stats.st_mode) && S_ISLNK(stats.st_mode))){
				    searchFilesRecursively(newPath);}
	    	}else{
	    		err = 1;
				perror(dp->d_name);
				return;
	    	}
        }
    }
    closedir(dr);
}



int main(int argc, char ** argv){
    progname = argv[0];
    int c;
    if (argc == 1){
		printf("usage: %s [filepath ...]\n", progname);
		return 2;
    }else{
		for (c=1; c<argc; c++){
	    	searchFilesRecursively(argv[c]);
	    }
	    if (err == 1){
	    	return 2;
	    }else{
	    	return res;
	    }
    }
}
