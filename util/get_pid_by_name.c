#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#define READ_BUF_SIZE 256

long * find_pid_by_name(char *pidName)
{
	DIR *dir;
	struct dirent *next;
	long *pidList = NULL;
	int i =0;
	
	dir = opendir("/proc");
	if (!dir)
	{
		printf("cannot open /proc\n");
		return NULL;
	}

	while((next = readdir(dir)) != NULL)
	{
		FILE *status;
		char filename[READ_BUF_SIZE];
		char buffer[READ_BUF_SIZE];
		char name[READ_BUF_SIZE];
	
		printf("d_name=%s\n", next->d_name);	
		if (strcmp(next->d_name, "..") == 0)
			continue;

		if (!isdigit(*next->d_name))
			continue;
		
		sprintf(filename, "/proc/%s/status", next->d_name);
		if (!(status = fopen(filename, "r")))
		{
			continue;
		}

		if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL)
		{
			fclose(status);
			continue;
		}
		printf("buffer=%s\n", buffer);
		
		fclose(status);

		sscanf(buffer, "%*s %s", name);
		if (strcmp(name, pidName) == 0){
			printf("Found it. %s", name);
			pidList = realloc(pidList, sizeof(long) * (i+2));
			memset(pidList, 0, sizeof(long) * (i+2));
			pidList[i]=strtol(next->d_name, NULL, 0);
		}
	}

	if (pidList)
	{
		return pidList;
	}

	return NULL;

}

//Do not malloc memory
long find_pid_by_name2(char *pidName)
{
    DIR *dir;
    struct dirent *next;
    long pidList = 0;
    int i =0;

    dir = opendir("/proc");
    if (!dir)
    {
        printf("cannot open /proc\n");
        return 0;
    }

    while((next = readdir(dir)) != NULL)
    {
        FILE *status;
        char filename[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        char name[READ_BUF_SIZE];

        //printf("d_name=%s\n", next->d_name);
        if (strcmp(next->d_name, "..") == 0)
            continue;

        if (!isdigit(*next->d_name))
            continue;

        sprintf(filename, "/proc/%s/status", next->d_name);
        if (!(status = fopen(filename, "r")))
        {
            continue;
        }

        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL)
        {
            fclose(status);
            continue;
        }
        //printf("buffer=%s\n", buffer);

        fclose(status);

        sscanf(buffer, "%*s %s", name);
        if (strcmp(name, pidName) == 0){
            //printf("Found it. %s", name);
            //pidList = (long *)malloc(sizeof(long) * (1 + i));
            //memset(pidList, 0, sizeof(long) * (1 + i));
            pidList=strtol(next->d_name, NULL, 0);
            break;
        }
    }

    return pidList;
}

int main(int argc, char *argv[])
{
	long *p = find_pid_by_name("bash");
	long pid = 0;
	
	if (p == NULL)
	{
		printf("DO NOTHING\n");
	}
	else
	{
		pid = p;
		free(p);
		printf("freep\n");
	}
	//sprintf(&pid, "%ld", p);
	printf("p=%ld, %ld\n", p, pid);
}
