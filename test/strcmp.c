/*************************************************************************
	> File Name: strcmp.c
	> Author: 
	> Mail: 
	> Created Time: 2018年01月10日 星期三 10时52分14秒
 ************************************************************************/

#include <stdio.h>

int main(){

    char *p = "114.14.12.13,2.5.6.7";
    char *str = "14.14.12.13";

    char *t = strstr(p, str);
    printf("t=[%s]\n, t+strlen(str)=[%c]\n", t, *(t+strlen(str)));

    if (strncmp(p, t, strlen(t)) == 0) {
        printf("equal t and p\n");
    } else {
        printf("not equal\n");
    }

    char *tt = strstr(p, "2.5.6.7");
    if (tt && (*(tt+strlen("2.5.6.7")) == '\0'
             || *(tt+strlen("2.5.6.7")) == ',')
             )
       
        printf("end\n");
    else 
        printf("Nonexist\n");
}
