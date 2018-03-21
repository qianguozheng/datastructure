#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv){
	char ip[]= "192.168.49.1";
	char *mask = "255.255.255.0";
	
	char mask2[] = "255.255.240.0";
	char gateway[] = "192.168.48.1";
	
	in_addr_t addr_t;
	struct in_addr addr;
	
//ip 地址  int和string的转换
	//1. str->binary
	inet_aton(ip, &addr);
	printf("addr=%x\n", addr);
	
	addr_t = inet_addr(ip);
	printf("addr_t=%x\n", addr_t);
	
	//2. binary->str
	char *ip_res;
	ip_res = inet_ntoa(addr);
	printf("ip=%s\n", ip_res);
	
	inet_ntop(AF_INET, &addr, ip_res, INET_ADDRSTRLEN);
	printf("ip=%s\n", ip_res);
	
	printf("==========================\n\n\n");
//ip 地址  mask的计算

	//转换后是大端字节序，转换为主机字节序htonl/ntohl
	in_addr_t addr_ip, addr_mask, addr_gw;
	addr_ip = inet_addr(ip);
	addr_mask = inet_addr(mask);
	addr_gw = inet_addr(gateway);
	
	printf("addr_ip=%x\naddr_mask=%x\n",addr_ip, addr_mask);
	
	printf("addr_ip&(~addr_mask)=%lu\n", ntohl(addr_ip) & ~(ntohl(addr_mask)));
	//printf("t1^(addr_gw)=%d\n", addr_t1 ^(addr_gw));
	
	in_addr_t addr_t3;
	addr_mask = inet_addr(mask2);
	printf("addr_ip&(~addr_mask)=%lu\n", ntohl(addr_ip) & ~(ntohl(addr_mask)));
	//printf("t1&(~t3)=%d\n", addr_t1 & ~(addr_t3));
	
}
