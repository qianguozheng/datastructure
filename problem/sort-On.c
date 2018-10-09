/*************************************************************************
	> File Name: sort-On.c
	> Author: 
	> Mail: 
	> Created Time: 2018年10月09日 星期二 10时08分47秒
 ************************************************************************/

#include<stdio.h>

void sortbyN(char *str) {
    if (str == NULL) return;
    const int N = 256;
    char b[N];

    //Init B[N]
    int i = 0;
    for (i=0; i< N; i++){
        b[i] = 0;
    }

    //Walk str
    for (i=0; i<strlen(str); i++){
        char key = str[i];
        ++b[key];
        printf("b[%c]=%d\n", key, b[key]);
    }

    int index = 0, j= 0;
    for (i = 0; i < N; ++i){
        for (j = 0; j < b[i]; ++j) {
            str[index] = i;
            printf("str[%d]=%c\n", index, str[index]);
            ++index;
        }
    }
}

int main(){
    char str[] = "bbaaccddoo";
    sortbyN(str);
    printf("sortByN=%s\n", str);

}
