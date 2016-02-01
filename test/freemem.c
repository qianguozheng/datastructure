#include <stdio.h>
#include <stdlib.h>

long check_memfree()
{
	FILE *fp = NULL;
	int i = 0;
	char freemem[128];
	char temp[128];
	char *ptr = NULL;
	long freeKB = 0;
	
	
	fp = fopen("/proc/meminfo", "r");
	memset(freemem, 0, sizeof(freemem));
	if (NULL == fp)
	{
		return -1;
	}
	else
	{
		for (i = 0; i < 2; i++)
		{
			fgets(temp, 128, fp);
			printf("temp=%s\n", temp);
			if (1 == i)
			{
				sprintf(freemem, "%s", temp);
			}
		}
		
		if (ptr = strstr(freemem, "kB"))
		{
			*ptr = '\0';
		}
		
		if (ptr = strstr(freemem, "MemFree:"))
		{
			ptr += strlen("MemFree:");
			freeKB = atol(ptr);
		}
		printf("monitor: system free mem %ld\n", freeKB);
	}
	
	return freeKB;
}


int main(char *argc, char *argv[])
{
	check_memfree();
	return 0;
}
