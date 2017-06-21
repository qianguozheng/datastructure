/*************************************************************************
	> File Name: short.c
	> Author: 
	> Mail: 
	> Created Time: 2017年05月02日 星期二 10时22分38秒
 ************************************************************************/

#include<stdio.h>

int main()
{
    unsigned short a = ~0;
    short b = ~0;
    printf("sizeof short=%d\n", sizeof(short));
    printf("sizeof unsigned short=%d\n", sizeof(unsigned short));
    printf("unsigned short=%d\n", a);
    printf("short=%d\n", b);
}
