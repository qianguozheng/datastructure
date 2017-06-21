/*************************************************************************
	> File Name: union.c
	> Author: 
	> Mail: 
	> Created Time: 2016年09月19日 星期一 14时27分02秒
 ************************************************************************/

#include<stdio.h>

struct car{
	union{
		char name[48];
		int price;
	};
};

#include <stdio.h>
#include <stdarg.h>
struct ex_type{
      union{
		   char name[16];
              double   un_double;
              char    un_char;
              int     un_int;
             
      };
};

int main()
{
    struct car test;
    
    sprintf(test.name, "%s" "Hi");
    printf("%s, %d\n", test.name, test.price);
    test.price = 10;
    printf("%s, %d\n", test.name, test.price);
    printf("union car=%d\n", sizeof(struct car));
    
        struct ex_type tt ;
    tt.un_char  = 'a';
    tt.un_int  = 10;
    tt.un_double = 10.5;
    
    printf("struct ex_type size = %d\n",sizeof(struct ex_type));
    printf("double size = %d\n",sizeof(double));
    printf("char   size = %d\n",sizeof(char));
    printf("int    size = %d\n",sizeof(int));
    printf("un_double  = %f\n",tt.un_double);
    printf("un_char  = %c\n",tt.un_char);
    printf("un_int   = %d\n",tt.un_int);

}

