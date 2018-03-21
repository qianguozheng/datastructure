/*************************************************************************
	> File Name: strtok.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月25日 星期一 17时47分19秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>
int main(){


    char buf[128];
    sprintf(buf, "%s", "hello world");
    char *ptr = NULL;
    printf("buf=%s\n", buf);
    ptr = strtok(buf, " ");
    if (NULL == ptr){

        printf("ptr is null\n");
        return 0;
    }
    printf("ptr=%s\n", ptr);
    return 0;
}
