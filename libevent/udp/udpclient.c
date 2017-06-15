#include <iostream>
#include <pthread.h>
#include <event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
int main()
{
	struct sockaddr_in in;
	int fd;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	struct in_addr s;
	
	memset(&in, 0, sizeof(struct sockaddr_in));
	
	in.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void *)&s);
	in.sin_addr.s_addr = s.s_addr;
	in.sin_port = htons(20010);
	
	string str = "I am Richard";
	sendto(fd, str.c_str(), str.size(), 0, (struct sockaddr *)&in, sizeof(struct sockaddr_in));
	
	return 0;
}
