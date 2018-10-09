/*************************************************************************
	> File Name: atoi.c
	> Author: 
	> Mail: 
	> Created Time: 2018年10月09日 星期二 11时05分07秒
 ************************************************************************/

#include<stdio.h>

int atoi(char *str){
    int i, n, sign;
    for (i=0; isspace(str[i]); i++);
    sign = (str[i]=='-')?-1:1;
    if (str[i]=='+'|| str[i]== '-') i++;
    for (n = 0; isdigit(str[i]); i++) {
        n = 10 * n + (str[i]-'0');
    }
    return sign * n;
}

int main(){
    int x = atoi(" -109");
    printf("x=%d\n", x);
    x = atoi("2 03");
    printf("x=%d\n", x);
}
