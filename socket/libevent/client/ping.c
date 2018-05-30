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

#include "ping.h"
#include "log.h"

//Refer to http://blog.csdn.net/yangzheng_yz/article/details/50593678


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

double sendping(struct sockaddr_in dest, int optlen){

	char buf[512];
	double rtt = -1;
	int len;
	
	struct icmp *pkt;
	int sock = create_icmp_socket();
	//printf("sock=%d\n", sock);
	
	memset(buf, 0, sizeof(buf));
	pkt = (struct icmp *)buf;
	
	gettimeofday((struct timeval *)pkt->icmp_data,NULL);

#define ICMP_ID 535
#define ICMP_SEQ 123

	pkt->icmp_id = ICMP_ID;
	pkt->icmp_seq = ICMP_SEQ;
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_cksum = inet_cksum((uint16_t *)pkt, sizeof(buf));

	len = sendto(sock, buf, DEFDATALEN + ICMP_MINLEN + optlen, 0, (struct sockaddr*)&dest, sizeof(dest));
	//printf("len=%d\n", len);

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
			//printf("c=%d, errno=%d\n", c, errno);
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
					//printf("get icmp reply, id=%d, seq=%d\n", pkt->icmp_id, pkt->icmp_seq);
					sendtime = (struct timeval *)pkt->icmp_data;
					
					rtt = ((&recvtime)->tv_sec - sendtime->tv_sec) * 1000 + ((&recvtime)->tv_usec - sendtime->tv_usec)/1000.0;
					printf("rtt=%f\n", rtt);
					break;
				}
			}
		}
	} while(0);
	close(sock);
	
	return rtt;
}

result_array network_delay(char *host){
	int sock;
	struct sockaddr_in dest;
	double arr[PKT_LEN][TEST_NUM], sum = 0, avg = 0, min = 0, max = 0;
	result_array ret;
	int fail_cnt = 0;
	
	//sock = create_icmp_socket();
	dest = init_dest(host);
	
	memset(arr, 0, sizeof(arr));
	memset(&ret, 0, sizeof(ret));
	
	int i=0, j=0;
	for (i=0; i<PKT_LEN; i++) {
		for (j =0; j < TEST_NUM; j++) {
			//arr[i][j] = sendping(sock, dest, 0); //default: 64 + [ 0, 64, 192 ]
			switch(i) {
				case 0:
					arr[i][j] = sendping(dest, 0);
					break;
				case 1:
					arr[i][j] = sendping(dest, 64);
					break;
				case 2:
					arr[i][j] = sendping(dest, 192);
					break;
				default:
					break;
			}
			
			//失败次数超过，则不上报这个mac
			if (fail_cnt > 5) {
				return ret;
			}
			//失败则返回-1
			if (arr[i][j] < 0) {
				fail_cnt++;
			}
			
			printf("Responsive=%lf\n", arr[i][j]);
			sum += arr[i][j];
			if (j > 0) {
				if (arr[i][j] > max) max = arr[i][j];
				if (arr[i][j] < min) min = arr[i][j];
			} else {
				min = max = arr[i][j];
			}
			
			sleep(1);
		}
		
		//统计结果
		avg = sum / TEST_NUM;
		sum = 0;
		switch(i) {
			case 0:
				ret.icmp[i].length = 0;
				break;
			case 1:
				ret.icmp[i].length = 64;
				break;
			case 2:
				ret.icmp[i].length = 128;
				break;
			default:
				break;
		}
		ret.icmp[i].avg = avg;
		ret.icmp[i].min = min;
		ret.icmp[i].max = max;
		printf("sock=%d [%d] max=%f min=%f avg=%f\n",sock, i, max, min, avg);
	}
	//printf("Average time is %f\n", sum/TEST_NUM);
	
	//close(sock);
	
	return ret;
}


#if 0
int main(){
	
	//network_delay("112.74.112.103");
	network_delay("192.168.3.199");
	
	return 0;
}
#endif
