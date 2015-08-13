/****************** client program *****************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
int main()
{
    int sockfd,ret,send_num,send_num_total=0;
    char buf[]="this is my socket data.";
    struct sockaddr_un server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sun_family=AF_UNIX;
    strcpy(server_addr.sun_path,"server.socket");
    sockfd=socket(AF_UNIX,SOCK_STREAM,0);
    if (sockfd<0)
    {
        printf("调用socket函数建立socket描述符出错！\n");
        exit(1);
    }
    printf("调用socket函数建立socket描述符成功！\n");
    ret=connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(server_addr));
    if (ret<0)
    {
        printf("调用connect函数失败，客户端连接服务器失败!\n ");
        exit(2);
    }
    printf("调用connect函数成功，客户端连接服务器成功！\n");
    while (1)
    {
        send_num=send(sockfd,buf,sizeof(buf),MSG_DONTWAIT);
        if (send_num<0)
            printf("调用send函数失败！");
        else
        {
            send_num_total+=send_num;
            printf("调用send函数成功，本次发送%d个字节，内容为：\"%s\"。目前共发送了%d个字节的数据。\n",send_num,buf,send_num_total);
        }
        sleep(2);
    }
}
