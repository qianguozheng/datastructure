/*************************************************************************
	> File Name: pointer.c
	> Author: 
	> Mail: 
	> Created Time: 2018年01月10日 星期三 17时19分55秒
 ************************************************************************/

#include<stdio.h>
#if 0
int myMalloc(char *s)
{
    s= (char *) malloc(100);
}

void main(){
    char *p = NULL;
    myMalloc(p);
    if (p) free(p);
}


void myMalloc(char **s){
    *s = (char *) malloc(100);
}

void main(){

    char *p = NULL;
    myMalloc(&p);
    if (p) free(p);
}
#endif

char *GetString2(void){
    char *p = "hello world";
    return p;
}

void main(){
    
    char *str = NULL;
    str = GetString2();

    printf("str=%si, %p\n", str);

}
