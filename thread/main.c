#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

void *printHello(void *args)
{
}

void *threadA(void* arg)
{
	char *buf = (char *)arg;
	
	printf("BUF=[%s]\n", buf);
	
	free(buf);
	//sleep(5);
	//printf("Buf=%s\n", buf);
}
int main(int argc, char *argv[])
{
	int rc, t;
	pthread_t thread;
	char buffer[100];
	char *buf = NULL;
	
	memset(buffer, 0, sizeof(buffer));
	
	sprintf(buffer, "%s", "Hello World@@@@@@@@@@@@\n");
	printf("buffer=%s\n", buffer);
	
	//WE MUST MALLOC a new heap area to store data to be transmited to 
	//another thead, otherwise, it will cause buffer operated by others.
	buf = (char *)malloc((strlen(buffer))+1);
	
	memset(buf, 0, strlen(buf)+1);
	
	memcpy(buf, buffer, strlen(buffer));
	rc = pthread_create(&thread, NULL, threadA, (void *)buf);
	if (rc)
	{
		printf("ERROR: %s\n", strerror(errno));
		return;
	}
	buf = NULL;
	pthread_detach(thread);
	memset(buffer, 0, sizeof(buffer));
	
	sleep(100);
}
