#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct obj{
	char name[8];
	int age;
}obj;

void obj_constructor(void *ob){

	strcpy(((obj *)ob)->name, "nick");
	((obj *)ob)->age = 10;
}

void obj_print(void *ob){
	printf("name %s\n",((obj *)ob)->name);
	printf("Age %d\n", ((obj *)ob)->age);
}

int main(){
	obj user1;
	obj_constructor(&user1);
	obj_print(&user1);
	return 0;
}