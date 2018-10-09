/*************************************************************************
	> File Name: macro.c
	> Author: 
	> Mail: 
	> Created Time: 2018年10月09日 星期二 09时47分31秒
 ************************************************************************/

#include<stdio.h>

#define MAX(a,b) (a) > (b) ? (a) : (b)

int main(){
    int max = MAX(2,5);
    printf("max=%d\n", max);
    max = MAX(10-2,8-1);

    printf("max=%d\n", max);
}
