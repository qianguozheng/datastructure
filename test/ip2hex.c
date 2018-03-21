/*************************************************************************
	> File Name: ip2hex.c
	> Author: 
	> Mail: 
	> Created Time: 2018年01月31日 星期三 18时21分31秒
 ************************************************************************/

#include<stdio.h>
#include <arpa/inet.h>
#include <string.h>
//34.251.47.238

int main(int argc, char *argv[]){

    if (argc != 2) {
        printf("argc not 2, retry\n");
    }
    in_addr_t t = inet_addr(argv[1]);
    printf("ip=%s, host=0x%X, network=  0x%X\n", argv[1], t, htonl(t));
    
    in_addr_t network = inet_addr("255.255.240.0");
    char buf[32];
    memset(buf, 0, sizeof(buf));
    //in_addr_t x = t & network;
    in_addr_t x = htonl(t) & htonl(network);
    inet_ntop(AF_INET, &x , buf, sizeof(buf));

    printf("t & network=%s\n", buf);

}
