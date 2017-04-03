#include <stdio.h>
#include <string.h>

int main(){
	char test[128];
	strcpy(test, "test/test");

	printf("%s\n", strcat(test, "test1.txt"));
	return 0;
}