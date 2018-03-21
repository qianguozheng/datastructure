#include <unistd.h>
#include <netinet/in.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <curl/curl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/wait.h>
 
#include <sys/socket.h>
#include <arpa/inet.h>


int sockfd = 0;
struct sockaddr_in binder;
#define USERANALYZER_PORT 9988

typedef struct msg_header{
	unsigned int length;
	unsigned int type;
} msg_header_t;

int main(){
	char buf[512], send_buf[512];
	msg_header_t *header = NULL;
	int msg_length = 0;
	
	memset(buf, 0, sizeof(buf));
	memset(send_buf, 0, sizeof(send_buf));
	
	//snprintf(buf, 512, "%s|%s\n", mac, str);
	snprintf(buf, 512, "%s", "hello|world\n");
	msg_length = sizeof(header) + strlen(buf);
	header = (msg_header_t *)send_buf;
	header->length = htonl(msg_length);
	header->type = htonl(0x1);
	
	memcpy(send_buf+sizeof(header), buf, strlen(buf));
	
	if (!sockfd) {
		sockfd = (int) socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0){
			sockfd = 0;
			return;
		}

		memset(&binder, 0, sizeof(binder));
		
		binder.sin_family = AF_INET; 
		binder.sin_addr.s_addr = INADDR_ANY; 
		binder.sin_port = htons(USERANALYZER_PORT);
		//Add by Richad 2017-6-8
		int on=1;
		if((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
		{  
			perror("setsockopt failed");  
			//exit(EXIT_FAILURE);  
		}
	}
	int bytes_sent = sendto(sockfd, send_buf, msg_length, 0,
                (struct sockaddr *)&binder,
                sizeof (struct sockaddr_in));
    if (bytes_sent <= 0) {
		perror("send error\n");
	}
	
	printf("bytes_sent=%d\n", bytes_sent);
}
