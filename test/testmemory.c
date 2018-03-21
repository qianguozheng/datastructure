/*************************************************************************
	> File Name: testmemory.c
	> Author: 
	> Mail: 
	> Created Time: 2017年12月26日 星期二 10时00分44秒
 ************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Return Good
//Parameter Good, need 二级指针，else Segment Fault
char * getmemory(char **str) {
    *str = malloc(100);
    //printf(str);
    return str;
}

//Good
char *getmemory2() {
	char * p  = malloc(100);
	return p;
}

//Segment fault
char *getmemory3(){
	char p[100];
	memset(p, 0, 100);
	return p;
}

int main(){
#if 0
    char *str = NULL, *t = NULL;
    t = getmemory(&str);
    memset(str, 0, 100);
    strcpy(str, "hello");
    printf(str);
    //memset(t, 0, 100);
    //strcpy(t, "hello");
    //printf(t);
#endif

//	char *p = getmemory2();
	char *p = getmemory3();
	//memset(p, 0, 100);
	strcpy(p, "helxxlo");
	printf(p);
    
}
