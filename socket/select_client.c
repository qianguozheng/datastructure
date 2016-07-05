#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8990

void block_request()
{
	
}

void async_request()
{
	
}

int main(int argc, char *argv[])
{
	fd_set rdfs;
	struct timeval tv;
	int n =  0, maxfd = 0;
	
	char buf[256];
	
	int sockfd = -1;
	struct sockaddr_in my_addr;
	struct sockaddr_in dest_addr;
	int destport = SERVER_PORT;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	memset(&dest_addr.sin_zero, 0, 8);
	
	if ((connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr))) == -1)
	{
		perror("connect");
		exit(1);
	}
	
	maxfd = sockfd;
	
	tv.tv_sec = 3;
	tv.tv_usec = 200;
	
	FD_ZERO(&rdfs);
	FD_SET(maxfd, &rdfs);
	
	memset(buf, 'X', sizeof(buf));
	send(maxfd, buf, strlen(buf), 0);
	do
	{
		
		n = select(maxfd+1, &rdfs, NULL, NULL, &tv);
		if (n < 0)
		{
			perror("something error happened");
			break;
		}
		else if (n == 0)
		{
			//perror("timeout");
			continue;
		}
		else
		{
			int len = recv(maxfd, buf, sizeof(buf), 0);
			printf("But=[%s], len=%d\n", buf, len);
			sleep(1);
			send(maxfd, buf, strlen(buf), 0);
			
		}
		
	}while (1);
	
	return 0;
}
