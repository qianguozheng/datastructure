#include <stdio.h>
#include <string.h>
#include <net/route.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
**add by zhoubp 2015.12.25
*/
int get_brlan_gateway(char *sgw, const char *devname)
{
	char   buff[256];
	char 	 interface[32];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;
	sgw[0] = '\0';

	FILE *fp = fopen("/proc/net/route", "r");

	while (fgets(buff, sizeof(buff), fp) != NULL) {
		printf("buff=[%s]\n", buff);
		if (nl) {
			if (sscanf(buff/*+ifl*/, "%s%lx%lx%X%d%d%d%lx",
				interface, &d, &g, &flgs, &ref, &use, &metric, &m)!=8) {
				fclose(fp);
				strcpy(sgw, "");
				printf("error sscanf\n");
				return -1;
			}
			if (flgs&RTF_UP) {
				dest.s_addr = d;
				gw.s_addr   = g;
				
				if (NULL == devname || !strcmp(interface, devname)) 
				{
					strcpy(sgw, (gw.s_addr==0 ? "" : inet_ntoa(gw)));
					printf("gateway=%s\n", sgw);
				}
				else
				{
					strcpy(sgw, "");
				}
				/*if (dest.s_addr == 0) {
					find_default_flag = 1;
					printf("error dest.s_addr ==  0\n");
					break;
				}*/
			}
		}
		nl++;
	}
	fclose(fp);
	
	if(devname != NULL && strcmp(interface, devname)) 
	{
		//yes, interface must be br-lan.
		return -1;
	}
	
	return 0;
}

int main(){
	char gateway[32] = {'\0'};
	get_brlan_gateway(gateway, NULL);
	return 0;
}
