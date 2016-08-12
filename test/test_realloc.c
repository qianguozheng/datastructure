/*************************************************************************
	> File Name: test_realloc.c
	> Author: 
	> Mail: 
	> Created Time: 2016年04月20日 星期三 15时16分16秒
 ************************************************************************/
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    char *p = NULL;
    p = (char *)realloc(p, 100);
    //memset(p, 0, 100);
    memcpy(p, "hello world", sizeof("hello world"));
    printf("p=%s\n", p);

    free(p);
    printf("end\n");
    return 0;
}
