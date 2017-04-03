#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(){


	int fd = open("test.txt", O_WRONLY , 0);
	char test[] = "test", test2[] = " test2";
	if(fd != -1){
		write(fd, test, 5);
		close(fd);
	}else{
		printf("Error\n");
	}
/*	
	fd = open("test.txt", O_CREAT|O_RDWR|O_APPEND|O_TRUNC, S_IRUSR|S_IWUSR);
	if(fd != -1){
		write(fd, test2, 6);
		close(fd);
	}else{
		printf("Error2\n");
	}
*/
	return 0;
}