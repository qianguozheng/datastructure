#include <stdio.h>
#include <stdlib.h>

#if 0
option trust_ip_range0 '192.168.3.138-192.168.3.142'
	option trust_ip_range1 '183.239.175.1-183.239.175.254'
	option trust_ip_range2 '112.90.246.87-112.90.246.87'
	option trust_ip_range3 '122.13.86.100-122.13.86.100'
	option trust_ip_range4 '112.91.125.129-112.91.125.129'
	option trust_ip_range5 '202.108.33.107-202.108.33.107'
	option trust_ip_range6 '58.250.125.37-58.250.125.37'
	option trust_ip_range7 '163.177.20.223-163.177.20.223'
	option trust_ip_range8 '163.177.63.95-163.177.63.95'
	option trust_ip_range9 '112.90.212.126-112.90.212.126'
	option trust_ip_range10 '36.250.76.204-36.250.76.204'
	option trust_ip_lenth '11'
#endif

int walkips(char *ips)
{
	char *start=NULL, *end=NULL;
	
	if (NULL == ips)
	{
		return -1;
	}
	
	start = ips;
	if (NULL != (end = strstr(ips, "-"))){
		*end = '\0';
		end++;
		//Compare start and end
		if (0 == strcasecmp(start, end)){
			//Only one IP address.
			printf("address=%s\n", start);
		}
		else { //Multi IP addressed.
			//仅仅处理最后一个ip地址
			char *start_last=NULL, *end_last=NULL;
			int start_num=0, end_num=0, tmp = 0, i=0;
			start_last = strrchr(start, '.');
			end_last = strrchr(end, '.');
			if (NULL == start_last || NULL ==end_last)
			{
				printf("start_last=%s, end_last=%s\n", start_last, end_last);
				return -2;
			}
			
			start_num = atoi(start_last+1);
			end_num = atoi(end_last+1);
			if (start_num > end_num)
			{
				tmp = start_num;
				start_num = end_num;
				end_num = tmp;
			}
			*start_last = '\0';
			char ip[16];
			memset(ip, 0, sizeof(ip));
			
			for (i = start_num; i < end_num; i++){
				sprintf(ip, "%s.%d", start, i);
				//Add this ip to list
				printf("Ip=%s\n", ip);
				memset(ip, 0, sizeof(ip));
			}
		}
	}
	return 0;
}

int main()
{
	char ips[64];
	memset(ips, 0, sizeof(ips));
	strcpy(ips, "183.239.175.1-183.239.175.254");
	walkips(ips);
	printf("===============================\n");
	//walkips("183.239.175.1-183.239.175.254");
	printf("===============================\n");
	//walkips("163.177.20.223-163.177.20.223");
	printf("===============================\n");
	//walkips("112.90.212.126-112.90.212.126");
	printf("===============================\n");
	//walkips("192.168.3.138-192.168.3.142");
	printf("===============================\n");
}
