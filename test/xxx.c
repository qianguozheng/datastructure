#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main()
{
	unsigned int ipwork = ntohl("192.168.48.1");
	printf("%x\n", ipwork);
}
