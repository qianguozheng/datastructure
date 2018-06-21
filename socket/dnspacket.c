#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define SRV_PORT 53
typedef unsigned short u16;
char srv_ip[] = "114.114.115.115";

typedef struct _DNS_HDR {
	u16 id;
	u16 tag;
	u16 numq;
	u16 numa;
	u16 numa1;
	u16 numa2;
} DNS_HDR;

typedef struct _DNS_QER {
	u16 type;
	u16 classes;
} DNS_QER;


int dns_query(char *server, char *domain, char *ip){
	int servfd, clifd, len = 0, i;
	struct sockaddr_in servaddr, addr;
	int socklen = sizeof(servaddr);
	char buf[BUF_SIZE];
	char *p;
	DNS_HDR *dnshdr = (DNS_HDR *)buf;
	DNS_QER *dnsqer = (DNS_QER *)(buf + sizeof(DNS_HDR));
	
	struct timeval send_timeout, recv_timeout;
	
	if ((clifd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("create socket error\n");
		return -1;
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_aton(server, &servaddr.sin_addr);
	servaddr.sin_port = htons(SRV_PORT);
	
	send_timeout.tv_sec = 5;
	send_timeout.tv_usec = 0;
	recv_timeout.tv_sec = 6;
	recv_timeout.tv_usec = 0;
	setsockopt(clifd, SOL_SOCKET, SO_SNDTIMEO, (char *)&send_timeout, sizeof(struct timeval));
	setsockopt(clifd, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_timeout, sizeof(struct timeval));
	
	memset(buf, 0, sizeof(buf));
	dnshdr->id = (u16)1;
	dnshdr->tag = htons(0x0100);
	dnshdr->numq = htons(1);
	dnshdr->numa = 0;
	
	sprintf(buf+sizeof(DNS_HDR)+1, "%s", domain);
	p = buf + sizeof(DNS_HDR) + 1; i = 0;
	while(p < buf + sizeof(DNS_HDR) + 1 + strlen(domain)) {
		if (*p == '.') {
			*(p-i-1) = i;
			i = 0;
		} else {
			i++;
		}
		p++;
	}
	
	*(p-i-1) = i;
	
	dnsqer = (DNS_QER *)(buf + sizeof(DNS_HDR) + 2 + strlen(domain));
	dnsqer->classes = htons(1);
	dnsqer->type = htons(1);
	
	//printf("send packet to dns server\n");
	len = sendto(clifd, buf, sizeof(DNS_HDR) + sizeof(DNS_QER) + strlen(domain) + 2, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	i = sizeof(struct sockaddr_in);
	//printf("waiting response from dns server\n");
	len = recvfrom(clifd, buf, BUF_SIZE, 0, (struct sockaddr *)&servaddr, &i);
	//printf("recv response from dns server\n");
	if (len < 0) {
		printf("recv error\n");
		return -1;
	}
	
	if (dnshdr->numa == 0) {
		printf("ack error\n");
		return -1;
	}
	
	p = buf + len - 4;
	printf("%s==>%u.%u.%u.%u\n", domain, (unsigned char)*p, (unsigned char)*(p+1), (unsigned char)*(p+2), (unsigned char)*(p+3));
	sprintf(ip, "%u.%u.%u.%u", (unsigned char)*p, (unsigned char)*(p+1), (unsigned char)*(p+2), (unsigned char)*(p+3));
	close(clifd);
	return 0;
}
int main(int argc, char *argv[]) {
	char domain[256];  //domain name to query
	char server[32];//dns server ip address
	char ip[16];
	
	memset(domain, 0, sizeof(domain));
	memset(server, 0, sizeof(server));
	memset(ip, 0, sizeof(ip));
	
	int ch;
	while((ch = getopt(argc, argv, "s:d:")) != -1) {
		switch(ch) {
			case 's':
				sprintf(server, "%s", optarg);
				break;
			case 'd':
				sprintf(domain, "%s", optarg);
				break;
			case '?':
				printf("unknow param\n");
				return 0;
		}
	}
	
	if (strlen(server) > 0)  {
		dns_query(server, domain, ip);
	} else {
		dns_query(srv_ip, domain, ip);
	}
	printf("IP:%s\n", ip);
	return 0;
}
