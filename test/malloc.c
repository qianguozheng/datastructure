#include <stdio.h>
#include <stdlib.h>

int main()
{
    int len = 1024*1024*200;
    char *ptr = NULL;

    ptr = malloc(len);
    memset(ptr, 0, len);
    if (ptr == NULL)
    {
        printf("Error\n");
        return -1;
    }
    printf("OK\n");
    return 0;
}
