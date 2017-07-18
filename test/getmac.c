#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>

char ap_mac[13];

int get_iface_mac(const char *ifname, char *ap_mac)
{
	int r, s;
	struct ifreq ifr;
	char *hwaddr;

	strcpy(ifr.ifr_name, ifname);

	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (-1 == s) {
		printf("get_iface_mac socket: %s", strerror(errno));
		return -1;
	}

	r = ioctl(s, SIOCGIFHWADDR, &ifr);
	if (r == -1) {
		printf("get_iface_mac ioctl(SIOCGIFHWADDR): %s", strerror(errno));
		close(s);
		return -1;
	}

	hwaddr = ifr.ifr_hwaddr.sa_data;
	close(s);
	
	snprintf(ap_mac, 13, "%02x%02x%02x%02x%02x%02x", 
			hwaddr[0] & 0xFF,
			hwaddr[1] & 0xFF,
			hwaddr[2] & 0xFF,
			hwaddr[3] & 0xFF,
			hwaddr[4] & 0xFF,
			hwaddr[5] & 0xFF
		);

	return 0;
}

int main(void)
{
	get_iface_mac("enp2s0", ap_mac);
	printf("mac=%s\n", ap_mac);
}
