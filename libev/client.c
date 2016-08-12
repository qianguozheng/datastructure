#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int sock;
	FILE *fp;
	struct fd_set fds;
	struct timeval timeout = {0, 0};//wait, nonblocking set to 0
	char buffer[256] = {0};
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0)
	{
		perror("socket");
		return -1;
	}
	bind("127.0.0.1", sock);
	
	
}
