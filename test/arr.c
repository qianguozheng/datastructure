#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char arr[8][16];

void modify (char (*p)[16]) {
	int i;
	for (i=0; i< 8; i++){
		memcpy(p, "hello1234567890", 15);
		printf("p=%p--[%d]--[%s]\n", p, i, p);
		++p;
	}
}

int main(int argc, char *argv[]){
	int i = 0;
	for (i=0; i< 8; i++){
		memset(arr[i], 0, 16);
	}
	modify(arr);
	
	for (i=0; i< 8; i++){
		printf("[%p] arr[%d]=%s\n",arr[i], i, arr[i]);
	}
	
	char *args[5];
	args[0] = "stainfoext";
	args[1] = "test";
	args[2] = "======";
	printf("args[0]=%s\n", args[0]);
	printf("args[1]=%s\n", args[1]);
	printf("args[2]=%s\n", args[2]);
	
	printf("argv[0]=%s\n", argv[0]);
	printf("argv[1]=%s\n", argv[1]);
	printf("argv[2]=%s\n", argv[2]);
	
}
