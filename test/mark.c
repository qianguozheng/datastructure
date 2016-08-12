#include <stdio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

int get_inf_network(const char *inf, char *network, int wlen, char *netmask, int mlen) 
{ 
	/*
	struct ifreq ifr; 
	int skfd = 0; 
	unsigned int ip, mask, net; 
	
	*network = 0; 
	*netmask = 0; 
	
	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	
	strncpy(ifr.ifr_name, inf, IF_NAMESIZE); 
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) 
	{
		close(skfd); return -1; 
	} 
	ip = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr; 
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) 
	{ 
		close(skfd); return -1; 
	} 
	mask = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr; 
	net = ip & mask; inet_ntop(AF_INET, &net, network, wlen); 
	inet_ntop(AF_INET, &mask, netmask, mlen); 
	//printf("net=%08X, ip=%08X, mask=%08X\n", net, ip, mask); 
	close(skfd); 
	*/
	strcpy(netmask, "255.255.240.0");
	
	return 0;
}
// Set the mark for each packet sent through this socket 
// @cip: client IP addr, little endian; @sock: set mark to it 
static int set_ip_mark(unsigned int cip, int sock)//(unsigned int cip, int sock) 
{
	static unsigned int _ipmask = 0; // little endian 
	int mark = 0; 
	
	//cip = ntohl(cip);
	if (0 == _ipmask) 
	{ // get LAN ip mask 
		char netmask[24], network[24]; 
		get_inf_network("br-lan", network, sizeof(network), netmask, sizeof(netmask)); 
		printf("netmask=%s\n", netmask);
		inet_pton(AF_INET, netmask, (void *)(&_ipmask)); 
		_ipmask = ntohl(_ipmask); 
		_ipmask = ~_ipmask; 
		//OUTPUT_BUG1("get LAN ip mask %u", _ipmask); 
		//debugs(17, 1, " Richard Fetching _ipmask" << _ipmask);
	}
	
	mark = (int)(cip & _ipmask); 
	mark = (mark << 16);
	
	printf("mark=%x\n", mark);
	//int ret = setsockopt(sock, SOL_SOCKET, SO_MARK, (void *)&mark,sizeof(mark)); 
	int ret;
	if (-1 == ret) 
	{ 
		//ERR_BUG1("setsockopt SO_MARK error, %d, %s", errno, strerror(errno)); 
	}
	return ret; 
}

int main()
{
	char ipstr[]="192.168.56.3";
	unsigned long t = inet_addr("192.168.56.3");
	printf("t=%x\n", t);
	printf("ntohl=%x\n", ntohl(t));
	set_ip_mark(ntohl(t), 1);
	set_ip_mark(t, 1);
}
