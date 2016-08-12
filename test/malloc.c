#include <stdio.h>
#include <stdlib.h>

int main()
{
    int len = 1024;
    char *ptr = NULL;

    ptr = malloc(len);
    memset(ptr, 0, len);
    memcpy(ptr, "world-hello, hi", 15);
    if (ptr == NULL)
    {
        printf("Error\n");
        return -1;
    }

    //This is not right in Squid programing, malloc len = X, realloc must > X, else could cause the squid 
    //memory mangement cause fatal error.
    ptr = realloc(ptr, 5);
    memcpy(ptr, "hello", 5); 

    printf("OK\n");
    return 0;
}
