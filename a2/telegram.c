#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

static char *progname;
int LINE_LENGTH=256;

static char *sub[256];

void convert_line(char * line){
    int i, j=0;
    char res[256] = {'\0'};
    char *c = " ";

    for (i = 0; i < strlen(line); i++){
        char * word = sub[(int)line[i]];
        if (word == NULL){
            strncat(res, &(line[i]), 1);
			j ++;
        }else{
            if (j >= 1 && (isspace(res[j-1]) == 0)){
				strncat(res, c, 1);
				j++;
            }
            strncat(res, word, sizeof(res) - strlen(res)-1);
			j += strlen(word);
            if (i < strlen(line)-1 && (isspace(line[i+1]) == 0)){
                strncat(res, c, 1);
				j++;
            }
        }
    }
    printf("%s",res);
}


void read_stdin(){
	char line[256];
	while (fgets(line, LINE_LENGTH + 1, stdin) != NULL){
		convert_line(line);
	}
}

void read_file(char * file_name){
	char line[256];
	FILE *file;
	
	file = fopen(file_name, "r");
	if (file == NULL){
		perror("Error opening file");    
	}
	while (fgets(line, LINE_LENGTH + 1, file) != NULL){
		convert_line(line);
	}
}

int main (int argc, char ** argv){
	sub[0] = "sb";
	sub['.'] = "STOP";
	sub[','] = "PAUSE";
	sub['?'] = "QUERY";

	progname = argv[0];
	int option, status = 0;
	int flag = 1;

	if (argc == 1 || (argc == 2 && strcmp(argv[1], "-") == 0)){
		read_stdin();
	}

	while ((option = getopt(argc, argv, "c:")) != EOF){
		switch (option)
		{
			case 'c':
				printf("");
				char j = optarg[0];
				sub[(int)j] = optarg+1;
			default:
				status=1;
				break;	
		}
	}
	if (optind == argc) {
		read_stdin();
		flag = 0;
	}
	while (optind < argc){
		if (strcmp(argv[optind], "-") == 0){
			read_stdin();
			flag=0;
		}else{
			read_file(argv[optind]);
			flag=0;
		}
		optind++;
	}

	if (flag == 1){
		status = 1;
		printf("usage: %s [-c substitution] [file ...]\n", progname);
	}	
	return status;
}


