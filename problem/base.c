/*************************************************************************
	> File Name: base.c
	> Author: 
	> Mail: 
	> Created Time: 2018年10月09日 星期二 10时00分56秒
 ************************************************************************/

#include<stdio.h>

typedef int(*PADD)(int, int);

int add(int c, int d){
    return c + d;
}

int main(){
    PADD padd = add;
    printf("%d\n",padd(3,4));
}
