#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	char buf[10]="";
	fd_set rdfds;
	struct timeval tv;
	int ret;
	
	FD_ZERO(&rdfds);
	FD_SET(0, &rdfds); //file descriptor 0 represent stdin input
	
	tv.tv_sec = 3;
	tv.tv_usec = 500;
	
	ret = select(1, &rdfds, NULL, NULL, &tv);
	if (ret < 0)
		printf("\n select");
	else if (0 == ret)
		printf("\n timeout");
	else
		printf("\n ret = %d", ret);
		
	if (FD_ISSET(1, &rdfds))
	{
		printf("\nreading");
		fread(buf, 9, 1, stdin);
	}
	
	write(1, buf, strlen(buf));
	printf("\n %d buf=%s==\n", strlen(buf), buf);
	
	return 0;
}
