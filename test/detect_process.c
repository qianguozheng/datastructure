/*************************************************************************
	> File Name: detect_process.c
	> Author: 
	> Mail: 
	> Created Time: Wed Jul 22 17:31:20 2015
 ************************************************************************/

#include<stdio.h>

int detect_process(char *process_name)
{
    FILE *ptr = NULL;
    char buff[512];
    char ps[128];
    sprintf(ps, "ps | grep %s| grep -v grep", process_name );
    strcpy(buff, "ABNORMAL");

    if ((ptr=popen(ps, "r")) != NULL)
    {
        while(fgets(buff, 512, ptr) != NULL)
        {
            printf("Buff=%s\n", buff);
        }
    }
    if (strcmp(buff, "ABNORMAL") == 0)
    {
        return -1;
    }
    pclose(ptr);
    return 0;
}

int main(int argc, char *argv[])
{   
    printf("%d\n", detect_process("polipox"));   
}
