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

/* not for assignment */
void print_xmerge(struct xmerge_param *ps){
	printf("outfile %s\n", ps->outfile);
	int i;
	for(i = 0; i < ps->infile_count; i++) printf(" Infile name %s\n", ps->infiles[i]);
	printf("infile_count %d\n", ps->infile_count);
	printf("oflags %d\n", ps->oflags);
	printf("Mode %d\n", ps->mode);
}

void Flags_constructor(Flag *flag){
	int i;
	for(i = 0; i < 8; i++){
		flag->oflags[i] = '\0';
		flag->mode[i] = '\0';
	}
	flag->hflag = 0;
}

int parsing_flags(char **argv, Flag *flag){ /*return postion of outfile*/
	int i = 1;
	do{
		if(argv[i][1] == 'h'){
			flag->hflag = 1;
		}else if(argv[i][1] == 'm'){
			strcpy(flag->mode, argv[++i]);
		}else if(argv[i][1] == 'a' || argv[i][1] == 'c' || argv[i][1] == 't' || argv[i][1] == 'e'){
			strcat(flag->oflags, &argv[i][1]);
		}

		i++;

	}while(argv[i][0] == '-');

	return i;
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
		}

		i++;
	}while(flags_arg[i] != '\0');

	return oflags;
}

void  xmerge_param_constructor(struct xmerge_param *ps, int argc, int outfile_postion,
						int *ofile_count, char **argv, Flag *flag){ 
	ps->outfile = argv[outfile_postion];
	ps->infiles = &argv[outfile_postion + 1];
	ps->infile_count = argc - (outfile_postion + 1);
	ps->oflags = oflags_handler(flag->oflags);
	ps->mode = (flag->mode[0] == '\0') ? S_IRUSR | S_IWUSR : strtoul(flag->mode, NULL, 8); /* may be deleted */
	ps->ofile_count = ofile_count;
}

int main(int argc, char ** argv) {
    long res;
    int files_read, outfile_postion;
    struct xmerge_param ps;	
/*
    printf("argc = %d\n", argc);
    int i;
    for(i = 0; i < argc; i++) printf("argv[%d]: %s\n", i, argv[i]);
*/
	/* TODO: Parse the argv here... 
	   Format:	   
	     - Usage: ./test_xmerge [flags] outfile infile infile2....	   
	     - After parsing, you should update ps.[attributeName]
	*/ 
	Flag flag;
	Flags_constructor(&flag);

	if(argc > 4){	
	    outfile_postion = parsing_flags(argv, &flag);
	    xmerge_param_constructor(&ps, argc, outfile_postion, &files_read, argv, &flag);
	}else{
		printf("Too few arguments\n");
		exit(0);
	}

	if(flag.hflag == 1) printf("Usage:  ./test_xmerge [flags] outfile infile infile2 ...\n");
	

    res = syscall(__NR_xmerge, &ps, sizeof(struct xmerge_param));
    if (res >= 0)
    {
        printf("In total, %d files have been successfully merged!\n",
               *(ps.ofile_count));
        printf("The total read size: %ld bytes.\n", res);
    } 
	else {
		/* Based on the errorno, print out appropriate error message */
	}
    //free(ps.infiles);
	
    return 0;
}
