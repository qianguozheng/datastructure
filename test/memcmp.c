#include <stdio.h>
#include <stdlib.h>

int main(){
	//char *json = NULL; // cause segmentfault
	char json[] = "hello";
	char json2[] = "hello world";
	if (0 == memcmp(json, json2, strlen(json2))){
		printf("equal\n");
	} else {
		printf("not equal\n");
	}
}
