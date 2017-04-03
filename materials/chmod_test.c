#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
	char mode[] = "644";

	mode_t ret = strtoul(mode, 0, 8);

	chmod("chmod.txt", ret);
	printf("mode %d\n", ret);
	return 0;
}