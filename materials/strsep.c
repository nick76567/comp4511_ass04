
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
	char *arr = "test test2 test3";

	char *arr1 = strdup(arr);
	char *token;
		
	while((token = strsep(&arr1, " ")) != NULL){
		printf("%s\n", token);
		//printf("%s\n", arr1);
	}

	free(arr1);
	return 0;
}

/*
#include <stdio.h>
#include <string.h>

int main() {

char *slogan = "together{kaliya} [namak]";
char *slow_gun = strdup(slogan);

char *token = strsep(&slow_gun, " ");

printf ("\n slow_gun: %s\n token: %s\n", slow_gun, token);

return 0;
}
*/