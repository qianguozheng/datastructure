#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define IFI_NAME 16
#define IFI_HADDR 8
#define IFI_ALIAS 1


struct ifi_info
{
	char ifi_name[IFI_NAME];	
	char ifi_haddr[IFI_HADDR];
	short ifi_hlen;
	short ifi_flags;
	short ifi_myflags;
	struct sockaddr* ifi_addr;
	struct sockaddr* ifi_brdaddr;
	struct sockaddr* ifi_dstaddr;
	struct ifi_info *ifi_next;
};

struct ifi_info *get_ifi_info(int,int);
struct ifi_info *Get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);
char *sock_ntop(struct sockaddr*, int);

int main(int argc, char **argv)
{
	struct ifi_info *ifi, *ifihead;
	struct sockaddr *sa;
	char *ptr;
	int i, family, doaliases;

	if (argc != 3) {
		fprintf(stderr, "usage:prifinfo <inet4|inet6> <doaliases>\n");
		return -1;
	}

	if (strcmp(argv[1], "inet4") == 0) {
		family = AF_INET;
	} else if (strcmp(argv[1], "inet6") == 0) {
		family = AF_INET6;
	}

	doaliases = atoi(argv[2]);

	for (ifihead = ifi = get_ifi_info(family, doaliases); ifi != NULL; ifi = ifi->ifi_next) {
		
		printf("ifi=%p\n", ifi);
		
		printf("%s:<" , ifi->ifi_name);
		if (ifi->ifi_flags & IFF_UP) printf("UP ");
		if (ifi->ifi_flags & IFF_BROADCAST) printf("BCAST ");
		if (ifi->ifi_flags & IFF_MULTICAST) printf("MCAST ");
		if (ifi->ifi_flags & IFF_LOOPBACK) printf("LOOP ");
		if (ifi->ifi_flags & IFF_POINTOPOINT) printf("P2P ");
		printf("> \n");
		
		if ((i = ifi->ifi_hlen) > 0) {
			ptr = ifi->ifi_haddr;
			do {
				printf("%s%x", (i == ifi->ifi_hlen) ? " " : ":", *ptr++);
			} while (--i > 0);
			printf("\n");
		}

		if ((sa = ifi->ifi_addr) != NULL) {
			printf(" IP addr: %s\n", sock_ntop(sa, sizeof(*sa)));
		}

		if((sa = ifi->ifi_brdaddr) != NULL) {
			printf(" broadcast addr: %s\n", sock_ntop(sa, sizeof(*sa)));
		}

		if ((sa = ifi->ifi_dstaddr) != NULL) {
			printf(" destination addr: %s\n", sock_ntop(sa, sizeof(*sa)));	
		}

		
	}
	
	free_ifi_info(ifihead);
	return 0;
}

struct ifi_info *get_ifi_info(int family, int doaliases)
{
	struct ifi_info *ifi, *ifihead, **ifipnext;
	int sockfd, len, lastlen, flags, myflags;
	char *ptr, *buf, lastname[128], *cptr;
	struct ifconf ifc;
	struct ifreq *ifr, ifrcopy;
	struct sockaddr_in *sinptr;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error:%s\n", strerror(errno));
		return NULL;
	}

	lastlen = 0;
	len = 100 * sizeof(struct ifreq);
	for (;;) {
		buf = malloc(len);
		ifc.ifc_len = len;
		ifc.ifc_buf = buf;
		if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
			if (errno != EINVAL || lastlen != 0) {
				fprintf(stderr, "ioctl error:%s\n", strerror(errno));
				return NULL;
			}
		} else {
			if (ifc.ifc_len == lastlen) break;
			lastlen = ifc.ifc_len;
		}
		len += 10 * sizeof(struct ifreq);
		free(buf);
	}

	ifihead = NULL;
	ifipnext = &ifihead;
	lastname[0] = 0;
	
	for (ptr = buf; ptr < buf + ifc.ifc_len;) {
		ifr = (struct ifreq*)ptr;
		switch(ifr->ifr_addr.sa_family) {
		case AF_INET6:
			len = sizeof(struct sockaddr_in6);
			break;	
		case AF_INET:
		default:
			len = sizeof(struct sockaddr_in);
			break;
		}

		printf("ifr_name=[%s] len=%d\n", ifr->ifr_name, len);
		//ptr += sizeof(ifr->ifr_name) + len;
		ptr += sizeof(struct ifreq);
		if (ifr->ifr_addr.sa_family != family) {
			continue;
		}

		myflags = 0;
		if ((cptr = strchr(ifr->ifr_name, ':')) != NULL) {
			*cptr = 0;
		}
		printf("lastname=[%s]\n", lastname);
		if (strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0) {
			if (doaliases == 0) {
				continue;
			}

			myflags = IFI_ALIAS;
			
		}
		memcpy(lastname, ifr->ifr_name, IFNAMSIZ);
		ifrcopy = *ifr;
		ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
		flags = ifrcopy.ifr_flags;
		if ((flags & IFF_UP) == 0) continue;
		
		printf("next is calloc lastname=%s\n", lastname);
		ifi = calloc(1, sizeof(struct ifi_info));
		
		*ifipnext = ifi;			/* prev points to this new one */
		ifipnext = &ifi->ifi_next;	/* pointer to next one goes here */
		
		ifi->ifi_flags = flags;
		ifi->ifi_myflags = myflags;
		memcpy(ifi->ifi_name, ifr->ifr_name, IFI_NAME);	
		ifi->ifi_name[IFI_NAME - 1] = 0;
		

		switch(ifr->ifr_addr.sa_family) {
		case AF_INET:	
			sinptr = (struct sockaddr_in*)&ifr->ifr_addr;
			if (ifi->ifi_addr == NULL) {
				ifi->ifi_addr = calloc(1, sizeof(struct sockaddr_in));
				memcpy(ifi->ifi_addr, sinptr, sizeof(struct sockaddr_in));
			}
			if (flags & IFF_BROADCAST) {
				ioctl(sockfd, SIOCGIFBRDADDR, &ifrcopy);
				sinptr = (struct sockaddr_in*)&ifrcopy.ifr_broadaddr;
				ifi->ifi_brdaddr = calloc(1, sizeof(struct sockaddr_in));
				memcpy(ifi->ifi_brdaddr, sinptr, sizeof(struct sockaddr_in));	
			}
			
			if (flags & IFF_POINTOPOINT) {
				ioctl(sockfd, SIOCGIFDSTADDR, &ifrcopy);
				sinptr = (struct sockaddr_in*)&ifrcopy.ifr_dstaddr;
				ifi->ifi_dstaddr = calloc(1, sizeof(struct sockaddr_in));
				memcpy(ifi->ifi_dstaddr, sinptr, sizeof(struct sockaddr_in));
			}

			break;
		default:
			break;
		}
	} 

	free(buf);
	return(ifihead);
}

void free_ifi_info(struct ifi_info *ifihead)
{
	struct ifi_info *ifi, *ifinext;

	for (ifi = ifihead; ifi != NULL; ifi = ifinext) {
		if (ifi->ifi_addr != NULL) free(ifi->ifi_addr);
		if (ifi->ifi_brdaddr != NULL) free(ifi->ifi_brdaddr);
		if (ifi->ifi_dstaddr != NULL) free(ifi->ifi_dstaddr);
		ifinext = ifi->ifi_next;
		free(ifi);
	}
}


char *sock_ntop(struct sockaddr* sa, int len)
{
	char portstr[7];
	static char str[128];

	switch(sa->sa_family) {
	case AF_INET: 
	{
		struct sockaddr_in *sin = (struct sockaddr_in*)sa;
		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) return NULL;
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ".%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return str;
	}
	}

	return NULL;
}

	












