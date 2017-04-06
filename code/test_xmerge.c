#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* To make grading easier, we use the same syscall ID */
#define __NR_xmerge 355
struct xmerge_param {
    char *outfile;
    char **infiles;
    int infile_count;
    int oflags;
    mode_t mode;
    int *ofile_count;
};

typedef struct Flag{
	char oflags[8];
	char mode[8];
	int hflag;
} Flag;

void Flags_constructor(Flag *flag){
	int i;
	for(i = 0; i < 8; i++){
		flag->oflags[i] = '\0';
		flag->mode[i] = '\0';
	}
	flag->hflag = 0;
}

int parsing_flags(int argc, char **argv, Flag *flag){ /*return postion of outfile*/
	int i;
	
	for(i = 1; i < argc; i++){
		if(argv[i][0] != '-') return i;
		
		if(argv[i][1] == 'h'){
			flag->hflag = 1;
		}else if(argv[i][1] == 'm'){
			strcpy(flag->mode, argv[++i]);
		}else{
			strcat(flag->oflags, &argv[i][1]);
		}

	}

	return i;
}

char **infiles_handler(char **infiles){
	int i = 0;
	
	do{
		if(access(infiles[i++], F_OK) == -1) return NULL;
	}while(infiles[i] != NULL);
	
	return infiles;
}

int oflags_handler(char *flags_arg){
	int i = 0, oflags = 0;
	do{
		if(flags_arg[i] == 'a'){
			oflags |= O_APPEND;
		}else if(flags_arg[i] == 'c'){
			oflags |= O_CREAT;
		}else if(flags_arg[i] == 't'){
			oflags |= O_TRUNC;
		}else if(flags_arg[i] == 'e'){
			oflags |= O_EXCL;
		}else{
			return -1;
		}

		i++;
	}while(flags_arg[i] != '\0');

	return oflags;
}

int mode_handler(char *mode){
	char accept_mode_value[] = "04567";
	int i, j;
	
	if(mode[0] == '\0') return (S_IRUSR | S_IWUSR);
	for(i = 0; i < 3; i++){
		for(j = 0; j < 5; j++){
			if(mode[i] == accept_mode_value[j]) break;
		}
		if(j == 5) return 1;
	}
	return strtoul(mode, NULL, 8);
}

void  xmerge_param_constructor(struct xmerge_param *ps, int argc, int outfile_postion,
						int *ofile_count, char **argv, Flag *flag){ 
	ps->outfile = argv[outfile_postion];
	ps->infiles = infiles_handler(&argv[outfile_postion + 1]);
	ps->infile_count = argc - (outfile_postion + 1);
	ps->oflags = oflags_handler(flag->oflags);
	ps->mode = mode_handler(flag->mode);
	ps->ofile_count = ofile_count;
}

int main(int argc, char ** argv) {
    long res;
    int files_read, outfile_postion;
    struct xmerge_param ps;	

	/* TODO: Parse the argv here... 
	   Format:	   
	     - Usage: ./test_xmerge [flags] outfile infile infile2....	   
	     - After parsing, you should update ps.[attributeName]
	*/ 
	Flag flag;
	Flags_constructor(&flag);	
	outfile_postion = parsing_flags(argc, argv, &flag);
	xmerge_param_constructor(&ps, argc, outfile_postion, &files_read, argv, &flag);

	if(flag.hflag == 1){
		printf("Usage:  ./test_xmerge [flags] outfile infile infile2 ...\n");
		return 0;
	}else if(argc < 4){
		printf("Error: Too few arguments");
		return 0;
	}
    
    res = syscall(__NR_xmerge, &ps, sizeof(struct xmerge_param));
	
    if (*ps.ofile_count == ps.infile_count)
    {
        printf("In total, %d files have been successfully merged!\n",
               *(ps.ofile_count));
        printf("The total read size: %ld bytes.\n", res);
    } 
	else {
		if(res == ENOENT){
			printf("Error: No such file.\n");
		}else if(res == EACCES){
			printf("Error: Permission denied.\n");
		}else if(res == EFAULT){
			printf("Error: Bad address.\n");
		}else if(res == EEXIST){
			printf("Error: File exists.\n");
		}else if(res == EINVAL){
			printf("Error: Invalid argument.\n");
		}else{
			printf("Error: terminated!.\n");
		}

	}
	
    return 0;
}
