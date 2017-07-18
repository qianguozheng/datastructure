/*************************************************************************
	> File Name: endian.c
	> Author: 
	> Mail: 
	> Created Time: 2017年06月12日 星期一 17时22分46秒
 ************************************************************************/

#include<stdio.h>

int endian(){

    unsigned int x=0x12345678;
    char *p = &x;

    if (*p == 0x78 )
        printf("little endian , low byte in low address\n");
    else{
        printf("big endian , high byte in high address\n");
    }
    printf("p=%x\n", *p);
}
int main(){

    endian();
}
