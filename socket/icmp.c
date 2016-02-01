#include <sys/types.h> /* this should be not needed, but ip_icmp.h lacks it */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>

#if 1
#include <netinet/in.h>
#include <net/if.h>
#endif

#ifndef __u8
#define __u8	u_int8_t
#define __u16	u_int16_t
#define __u32	u_int32_t
#endif

/*
 * IP header
 */
struct myiphdr {
#if 0//defined(__LITTLE_ENDIAN_BITFIELD)
        __u8    ihl:4,
                version:4;
#elif 1//defined (__BIG_ENDIAN_BITFIELD)
        __u8    version:4,
                ihl:4;

#endif
        __u8    tos;
        __u16   tot_len;
        __u16   id;
        __u16   frag_off;
        __u8    ttl;
        __u8    protocol;
        __u16   check;
        __u32   saddr;
        __u32   daddr;
};

struct myicmphdr
{
	__u8          type;
	__u8          code;
	__u16         checksum;
	union
	{
		struct
		{
			__u16   id;
			__u16   sequence;
		} echo;
		__u32   gateway;
	} un;
};

#ifndef ICMPHDR_SIZE
#define ICMPHDR_SIZE	sizeof(struct myicmphdr)
#endif
#ifndef IPHDR_SIZE
#define IPHDR_SIZE	sizeof(struct myiphdr)
#endif

int _icmp_seq = 0;
int opt_icmptype = 8;
int opt_icmpcode = 0;

int icmp_cksum = -1;

struct sockaddr_in
	icmp_ip_src,
	icmp_ip_dst,
	icmp_gw,
	local,
	remote;
int 	src_ttl		= 64,
	src_id		= -1; /* random */

int 	opt_safe	= 0,
		eof_reached = 0,
			opt_rand_dest	= 0,
	opt_rand_source	= 0,
	opt_rawipmode	= 0,
	opt_icmpmode	= 1,
	opt_udpmode	= 0;

int
	sockraw;
	
	struct in_addr *
wd_gethostbyname(const char *name)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct in_addr *h_addr;
	int  s;

	/* XXX Calling function is reponsible for free() */

	h_addr = malloc(sizeof(struct in_addr));
	//h_addr = (struct in_addr *)malloc(sizeof(struct in_addr));
	//h_addr->s_addr = 0x78c58112;
	//return h_addr;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;    /* Allow IPv4 ,No IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(name, NULL, &hints, &result);
	if (s != 0) {
		//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		free(h_addr);
		return NULL;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
		//const char *ip = inet_ntoa(addr->sin_addr);
		//fprintf(stdout, "ip=%s, name=%p\n", ip, rp->ai_canonname);
		*h_addr = addr->sin_addr;
		break;
	}
	
	freeaddrinfo(result);
	
	//mark_online();

	return h_addr;
}

int resolve_addr(struct sockaddr * addr, char *hostname)
{
	struct  sockaddr_in *address;
	struct  hostent     *host;

	address = (struct sockaddr_in *)addr;

	memset(address, 0, sizeof(struct sockaddr_in));
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = inet_addr(hostname);

	if ( (int)address->sin_addr.s_addr == -1) {
		host = gethostbyname(hostname);
		if (host) {
			printf("host->addr=%s\n", host->h_addr);
			memcpy(&address->sin_addr, host->h_addr,
				host->h_length);
			return 0;
		} else {
			return -1;
		}
	}
	return 0;
}
/* Like resolve_addr but exit on error */
void resolve(struct sockaddr *addr, char *hostname)
{
	if (resolve_addr(addr, hostname) == -1) {
		fprintf(stderr, "Unable to resolve '%s'\n", hostname);
		exit(1);
	}
}

unsigned int ip_tos = 0;
int opt_fragment = 0;
int ip_optlen = 0;
char ip_opt[40];
int opt_mf = 0;
int opt_df = 0;

void send_ip (char* src, char *dst, char *data, unsigned int datalen,
		int more_fragments, unsigned short fragoff, char *options,
		char optlen)
{
	char		*packet;
	int		result,
			packetsize;
	struct myiphdr	*ip;

	printf("IPHDR_SIZE=%d, optlen=%d, datalen=%d\n", IPHDR_SIZE, optlen, datalen);
	
	packetsize = IPHDR_SIZE + optlen + datalen;
	if ( (packet = malloc(packetsize)) == NULL) {
		perror("[send_ip] malloc()");
		return;
	}

	memset(packet, 0, packetsize);
	ip = (struct myiphdr*) packet;

	/* copy src and dst address */
	printf("src=%s\n", inet_ntoa(local.sin_addr) );
	printf("dest=%s\n", inet_ntoa(remote.sin_addr));
	memcpy(&ip->saddr, &local.sin_addr, sizeof(ip->saddr));
	memcpy(&ip->daddr, &remote.sin_addr, sizeof(ip->daddr));

	/* build ip header */
	ip->version	= 4;
	ip->ihl		= (IPHDR_SIZE + optlen + 3) >> 2;
	ip->tos		= ip_tos;

#if defined OSTYPE_DARWIN || defined OSTYPE_FREEBSD || defined OSTYPE_NETBSD || defined OSTYPE_BSDI
/* FreeBSD */
/* NetBSD */
	ip->tot_len	= packetsize;
#else
/* Linux */
/* OpenBSD */
	ip->tot_len	= htons(packetsize);
#endif

	if (!opt_fragment)
	{
		ip->id		= (src_id == -1) ?
			htons((unsigned short) rand()) :
			htons((unsigned short) src_id);
	}
	else /* if you need fragmentation id must not be randomic */
	{
		/* FIXME: when frag. enabled sendip_handler shold inc. ip->id */
		/*        for every frame sent */
		ip->id		= (src_id == -1) ?
			htons(getpid() & 255) :
			htons((unsigned short) src_id);
	}

#if defined OSTYPE_DARWIN || defined OSTYPE_FREEBSD || defined OSTYPE_NETBSD | defined OSTYPE_BSDI
/* FreeBSD */
/* NetBSD */
	ip->frag_off	|= more_fragments;
	ip->frag_off	|= fragoff >> 3;
#else
/* Linux */
/* OpenBSD */
	ip->frag_off	|= htons(more_fragments);
	ip->frag_off	|= htons(fragoff >> 3); /* shift three flags bit */
#endif

	ip->ttl		= src_ttl;
	if (opt_rawipmode)	ip->protocol = 6;
	else if	(opt_icmpmode)	ip->protocol = 1;	/* icmp */
	//else if (opt_udpmode)	ip->protocol = 17;	/* udp  */
	else			ip->protocol = 6;	/* tcp  */
	ip->check	= 0; /* always computed by the kernel */

	/* copies options */
	if (options != NULL)
		memcpy(packet+IPHDR_SIZE, options, optlen);

	/* copies data */
	memcpy(packet + IPHDR_SIZE + optlen, data, datalen);
	
	int opt_debug = 1;
    if (opt_debug == 1)
    {
        unsigned int i;

        for (i=0; i<packetsize; i++)
            printf("%.2X ", packet[i]&255);
        printf("\n");
    }
	result = sendto(sockraw, packet, packetsize, 0,
		(struct sockaddr*)&remote, sizeof(remote));
	
	printf("Result=%d\n", result);
	if (result == -1 && errno != EINTR && !opt_rand_dest && !opt_rand_source) {
		perror("[send_ip] sendto");
		if (close(sockraw) == -1)
			perror("[ipsender] close(sockraw)");
		//if (close_pcap() == -1)
		//	printf("[ipsender] close_pcap failed\n");
		exit(1);
	}

	free(packet);

	/* inc packet id for safe protocol */
	if (opt_safe && !eof_reached)
		src_id++;
}



void send_ip_handler(char *packet, unsigned int size)
{
	//ip_optlen = ip_opt_build(ip_opt);

	printf("ip_optlen=%d\n", ip_optlen);
	

	if (!opt_fragment)
	{
		printf(" !opt_fragment, opt_mf=%d, opt_df=%d\n", opt_mf, opt_df);
		unsigned short fragment_flag = 0;

		//if (opt_mf) fragment_flag |= MF; /* more fragments */
		//if (opt_df) fragment_flag |= DF; /* dont fragment */
		
		//printf("size=%d, fragment_flag=%d, ip_frag_offset=%d, ip_opt=%s, ip_optlen=%d\n",
		//		size, fragment_flag, ip_frag_offset, ip_opt, ip_optlen);
		
		send_ip((char*)&local.sin_addr,
			(char*)&remote.sin_addr,
			packet, size, fragment_flag, 0,
			ip_opt, ip_optlen);
	}

}

/*
 * from R. Stevens's Network Programming
 */
__u16 cksum(__u16 *buf, int nbytes)
{
	__u32 sum;

	sum = 0;
	while (nbytes > 1) {
		sum += *buf++;
		nbytes -= 2;
	}

	if (nbytes == 1) {
		sum += *((__u8*)buf);
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	/* return a bad checksum with --badcksum option */
	//if (opt_badcksum) sum ^= 0x5555;

	return (__u16) ~sum;
}


void send_icmp_echo(void)
{
	char *packet, *data;
	struct myicmphdr *icmp;

	printf("send_icmp_echo\n");
	packet = malloc(ICMPHDR_SIZE);
	if (packet == NULL) {
		perror("[send_icmp] malloc");
		return;
	}

	memset(packet, 0, ICMPHDR_SIZE);

	icmp = (struct myicmphdr*) packet;
	data = packet + ICMPHDR_SIZE;

	/* fill icmp hdr */
	icmp->type = opt_icmptype;	/* echo replay or echo request */
	icmp->code = opt_icmpcode;	/* should be indifferent */
	icmp->checksum = 0;
	icmp->un.echo.id = getpid() & 0xffff;
	icmp->un.echo.sequence = _icmp_seq;

	/* data */
	//data_handler(data, data_size);

	/* icmp checksum */
	if (icmp_cksum == -1)
	{
		//icmp->checksum = cksum((u_short*)packet, ICMPHDR_SIZE + data_size);
		icmp->checksum = cksum((u_short*)packet, ICMPHDR_SIZE);
	}
	else
		icmp->checksum = icmp_cksum;

	/* adds this pkt in delaytable */
	
	//if (opt_icmptype == ICMP_ECHO)
	//	delaytable_add(_icmp_seq, 0, time(NULL), get_usec(), S_SENT);

	/* send packet */
	send_ip_handler(packet, ICMPHDR_SIZE);
	free (packet);

	_icmp_seq++;
}


int open_sockraw()
{
	int s;

	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (s == -1) {
		perror("[open_sockraw] socket()");
		return -1;
	}

	return s;
}


int main()
{
	resolve((struct sockaddr*)&remote, "112.74.112.103");
	printf(" dest=%s\n", inet_ntoa(remote.sin_addr));
	
	resolve((struct sockaddr*)&local, "192.168.202.102");
	printf("src=%ss\n", inet_ntoa(local.sin_addr));
	printf(" dest=%s\n", inet_ntoa(remote.sin_addr));
	
	sockraw = open_sockraw();
	if (sockraw == -1) {
		printf("[main] can't open raw socket\n");
		exit(1);
	}
	
	char buff[512];
	memset(buff, 0, sizeof(buff));
	while(1)
	{
		send_icmp_echo();
		//read(sockraw, buff, 512);
		//printf(buff);
		memset(buff, 0, 512);
		sleep(1);
	}
}
