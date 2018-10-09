/*************************************************************************
	> File Name: sort.c
	> Author: 
	> Mail: 
	> Created Time: 2018年10月09日 星期二 10时48分00秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>
void sort(char *str){
    if (str == NULL){
        return;
    }

    const int N = 256+1;
    char b[N];
    int i, j, index;
    int len = strlen(str);
    for (i=0; i< N; i++) b[i] = 0;

    for (j = 0; j < len; j++){
        char key = str[j];
        b[key]++;
    }


    for (i = 0; i < N; i++) {
        for (j =0; j < b[i]; j++){
            str[index++] = i;
        }
    }
}

int main() {
    char str[] = "nnccdddooooa";
    sort(str);
    printf("str=%s\n", str);
}
