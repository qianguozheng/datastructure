/*************************************************************************
	> File Name: revert.c
	> Author: 
	> Mail: 
	> Created Time: 2018年10月09日 星期二 09时50分42秒
 ************************************************************************/

#include<stdio.h>

char* revese(char *str){
    char *output = NULL;
    int len = strlen(str)+1;
    int i = 0,j =0;
    output = (char *)malloc(len);
    memset(output, 0, len);

    for (i=strlen(str)-1; i>= 0;i--) {
        output[j++] = str[i];
    }
    return output;
}

int main(){

    printf("reverse=%s\n", revese("helloworld"));
    printf("reverse=%s\n", revese("testmethod"));
}
