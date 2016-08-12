#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <net/if.h>

char *ipaddr = NULL;
char * get_iface_ip(const char *ifname)
{

	struct ifreq if_data;
	struct in_addr in;
	char *ip_str;
	int sockd;
	u_int32_t ip;

	/* Create a socket */
	if ((sockd = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf( "socket(): %s", strerror(errno));
		return NULL;
	}

	/* Get IP of internal interface */
	strcpy (if_data.ifr_name, ifname);

	/* Get the IP address */
	if (ioctl (sockd, SIOCGIFADDR, &if_data) < 0) {
		printf( "ioctl(): SIOCGIFADDR %s", strerror(errno));
		return NULL;
	}
	memcpy ((void *) &ip, (void *) &if_data.ifr_addr.sa_data + 2, 4);
	in.s_addr = ip;

	ip_str = inet_ntoa(in);
	close(sockd);
	return strdup(ip_str);
}

int get_mac_via_arptable_by_ip(/*char *ip, char *mac*/)
{
	char   buff[256] = {'\0'};

	char ip_address[32] = {'\0'}; // Obviously more space than necessary, just illustrating here.
	int hw_type;
	int flags;
	char mac_address[32] = {'\0'};
	char mask[32] = {'\0'};
	char device[32] = {'\0'};

	FILE *fp = fopen("/proc/net/arp", "r");

	fgets(buff, sizeof(buff), fp);    // Skip the first line (column headers).

	while (fgets(buff, sizeof(buff), fp) != NULL) 
	{
		//if(sscanf(buff, "%s 0x%x 0x%x %s %s %s\n", 
		//	ip_address, &hw_type, &flags, mac_address, mask, device) != 6) 
		//{
		//	fclose(fp);
			//strcpy(mac, "");
		//	return -1;
		//}

		printf("br-lan ip_address[%s]", ip_address);
         
        //if (!strcmp(ip_address, ip) && !strcmp(device, "br-lan")) 
		{
		//	strcpy(mac, mac_address);
		}
	}
	fclose(fp);
	
	return 0;
}
int main()
{
	sleep(30);
	ipaddr = get_iface_ip("eth1");
	printf("ip=%s\n", ipaddr);
	
	get_mac_via_arptable_by_ip();
	
	free(ipaddr);
	
	sleep(60);
}
