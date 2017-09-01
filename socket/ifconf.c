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
#include <linux/if_packet.h>
#include <linux/if_ether.h>

int main()
{//想要获取当前网口网线插入状态，需要用到ifreq结构体，获取网卡的信息，然后socket结合网卡驱动的ioctl，就可以得到与网线插入状态相关的数据。
    int number;                 
    char *tmpbuf;               
    struct ifconf ifc;          //用来保存所有接口信息的   
    struct ifreq buf[16];       //这个结构定义在net/if.h，用来配置ip地址，激活接口，配置MTU等接口信息
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;

    int sockfd;
    if(-1 == (sockfd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL))))     
    {
        perror("socket build !");
        return 1;
    } 
    if(-1 == ioctl(sockfd,SIOCGIFCONF,(char *)&ifc))//SIOCGIFCONF用来获取所有配置接口的信息，将所获取的信息保存到ifc里。
    {
        perror("SIOCGIFCONF !");
        return 1;
    }
    
    number = ifc.ifc_len / sizeof(struct ifreq);
    printf("the interface number is %d \n",number);
    int tmp;
    for(tmp = number;tmp > 0;tmp--)
    {
        printf("the interface name is %s\n",buf[tmp].ifr_name);

        /* 接口的状态信息获取 */
        if(0 == ioctl(sockfd,SIOCGIFFLAGS,(char *)&buf[tmp]))
        { 
            if(IFF_UP == buf[tmp].ifr_flags)
                printf("UP\n");
            else 
                printf("DOWN");
        } 

        /* IP地址的获取 */
        if(0 == ioctl(sockfd,SIOCGIFADDR,(char *)&buf[tmp]))
        {
            tmpbuf = (char *)inet_ntoa(((struct sockaddr_in*) (&buf[tmp].ifr_addr))->sin_addr);
            printf("IPAdress :%s \n",tmpbuf);
        }   

        /* 子网掩码的获取 */
        if(0 == ioctl(sockfd,SIOCGIFNETMASK,(char *)&buf[tmp]))   
        {
            tmpbuf = (char *)inet_ntoa(((struct sockaddr_in*) (&buf[tmp].ifr_addr))->sin_addr);
            printf("netmask:%s \n",tmpbuf);
        }  

        /* 广播地址的获取 */
        if(0 == ioctl(sockfd,SIOCGIFBRDADDR,(char *)&buf[tmp]))
        {
            tmpbuf = (char *)inet_ntoa(((struct sockaddr_in*)(&buf[tmp].ifr_addr))->sin_addr);
            printf("broadcast address:%s \n",tmpbuf);
        } 

        /* MAC地址的获取 */
        if(0 == ioctl(sockfd,SIOCGIFHWADDR,(char *)&buf[tmp]))
        {
 printf("MAC:%02x:%02x:%02x:%02x:%02x:%02x\n\n",
                    (unsigned char) buf[tmp].ifr_hwaddr.sa_data[0],
                    (unsigned char) buf[tmp].ifr_hwaddr.sa_data[1],
                    (unsigned char) buf[tmp].ifr_hwaddr.sa_data[2],
                    (unsigned char) buf[tmp].ifr_hwaddr.sa_data[3],
                    (unsigned char) buf[tmp].ifr_hwaddr.sa_data[4],
                    (unsigned char) buf[tmp].ifr_hwaddr.sa_data[5]);
        } 
    }
}
