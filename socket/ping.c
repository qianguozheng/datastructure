#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>

//Refer to http://blog.csdn.net/yangzheng_yz/article/details/50593678

#if 0
#define ICMP_ECHOREPLY 0
#define ICMP_ECHO

#define BUFSIZE 1500
#define DEFAULT_LEN 56

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct icmphdr{
	
}
#endif


#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
# define BB_BIG_ENDIAN 1
# define BB_LITTLE_ENDIAN 0
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
# define BB_BIG_ENDIAN 0
# define BB_LITTLE_ENDIAN 1
#elif defined(_BYTE_ORDER) && _BYTE_ORDER == _BIG_ENDIAN
# define BB_BIG_ENDIAN 1
# define BB_LITTLE_ENDIAN 0
#elif defined(_BYTE_ORDER) && _BYTE_ORDER == _LITTLE_ENDIAN
# define BB_BIG_ENDIAN 0
# define BB_LITTLE_ENDIAN 1
#elif defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
# define BB_BIG_ENDIAN 1
# define BB_LITTLE_ENDIAN 0
#elif defined(BYTE_ORDER) && BYTE_ORDER == LITTLE_ENDIAN
# define BB_BIG_ENDIAN 0
# define BB_LITTLE_ENDIAN 1
#elif defined(__386__)
# define BB_BIG_ENDIAN 0
# define BB_LITTLE_ENDIAN 1
#else
# error "Can't determine endianness"
#endif

enum {
	DEFDATALEN = 56,
	MAXIPLEN = 60,
	MAXICMPLEN = 76,
	MAX_DUP_CHK = (8 * 128),
	MAXWAIT = 10,
	PINGINTERVAL = 1, /* 1 second */
	pingsock = 0,
};


uint16_t inet_cksum(uint16_t *addr, int nleft)
{
	/*
	 * Our algorithm is simple, using a 32 bit accumulator,
	 * we add sequential 16 bit words to it, and at the end, fold
	 * back all the carry bits from the top 16 bits into the lower
	 * 16 bits.
	 */
	unsigned sum = 0;
	while (nleft > 1) {
		sum += *addr++;
		nleft -= 2;
	}

	/* Mop up an odd byte, if necessary */
	if (nleft == 1) {
		if (BB_LITTLE_ENDIAN)
			sum += *(uint8_t*)addr;
		else
			sum += *(uint8_t*)addr << 8;
	}

	/* Add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
	sum += (sum >> 16);                     /* add carry */

	return (uint16_t)~sum;
}

int create_icmp_socket(void){
	int sock = socket(AF_INET, SOCK_RAW, 1); /* 1 == ICMP*/
	if (sock < 0){
		return -1;
	}
	printf("icmp socket=%d\n", sock);
	return sock;
}

struct sockaddr_in init_dest(char *ipaddr){
	struct sockaddr_in dest;
	
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = ntohs(0);
	dest.sin_addr.s_addr = inet_addr(ipaddr);
	
	return dest;
}

double sendping(int sock, struct sockaddr_in dest){

	char buf[512];
	double rtt = -1;
	int len;
	
	struct icmp *pkt;
	
	memset(buf, 0, sizeof(buf));
	
	//sock = create_icmp_socket();
	printf("sock=%d\n", sock);

	pkt = (struct icmp *)buf;
	
	gettimeofday((struct timeval *)pkt->icmp_data,NULL);

#define ICMP_ID 535
#define ICMP_SEQ 123

	pkt->icmp_id = ICMP_ID;
	pkt->icmp_seq = ICMP_SEQ;
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_cksum = inet_cksum((uint16_t *)pkt, sizeof(buf));

	len = sendto(sock, buf, DEFDATALEN + ICMP_MINLEN, 0, (struct sockaddr*)&dest, sizeof(dest));
	printf("len=%d\n", len);

	//receive
	memset(buf, 0, sizeof(buf));
	do {
		fd_set rset;
		int maxfd = sock+1;
		struct timeval tv;
		
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		
		FD_ZERO(&rset);
		FD_SET(sock, &rset);
		
		int ret = select(maxfd, &rset, NULL, NULL, &tv);
		if (ret < 0){
			printf("timeout\n");
			break;
		} else if (0 == ret){
			printf("ret = 0, error\n");
			break;
		}
		if (FD_ISSET(sock, &rset)){
			int c = recv(sock, buf, sizeof(buf), 0);
			printf("c=%d, errno=%d\n", c, errno);
			if (c < 0) {
				if (errno != EINTR)
				continue;
			}
			if (c >= 76) {
				struct timeval recvtime, *sendtime;
				gettimeofday(&recvtime, NULL);
				struct iphdr *iphdr = (struct iphdr *) buf;
				pkt = (struct icmp *)(buf+(iphdr->ihl << 2));
				if (pkt->icmp_type == ICMP_ECHOREPLY &&
					pkt->icmp_id == ICMP_ID &&
					pkt->icmp_seq == ICMP_SEQ){
					printf("get icmp reply, id=%d, seq=%d\n", pkt->icmp_id, pkt->icmp_seq);
					sendtime = (struct timeval *)pkt->icmp_data;
					
					rtt = ((&recvtime)->tv_sec - sendtime->tv_sec) * 1000 + ((&recvtime)->tv_usec - sendtime->tv_usec)/1000.0;
					printf("rtt=%f\n", rtt);
					break;
				}
			}
		}
	} while(0);
	//close(s);
	
	return rtt;
}

#define TEST_NUM 10

double network_delay(char *host){
	int sock;
	struct sockaddr_in dest;
	double arr[TEST_NUM], sum = 0;
	
	sock = create_icmp_socket();
	
	dest = init_dest(host);
	
	memset(arr, 0, sizeof(arr));
	for (int i =0; i < TEST_NUM; i++){
		arr[i] = sendping(sock, dest);
		printf("Responsive=%lf\n", arr[i]);
		sum += arr[i];
		sleep(1);
	}
	printf("Average time is %f\n", sum/TEST_NUM);
	
	close(sock);
	
	return sum/TEST_NUM;
}


int main(){
	
	network_delay("112.74.112.103");
	
	return 0;
}
