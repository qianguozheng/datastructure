
/****************** server program *****************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
int main()
{
    int sockfd,newfd,ret,recv_num,recv_num_total=0;
    char buf[50];
    struct sockaddr_un server_addr;
    remove("server.socket");/*不管有没有，先删除一下，否则如果该文件已经存在的的话，bind会失败。*/
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
    ret=bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(server_addr));
    if (ret<0)
    {
        printf("调用bind函数绑定套接字与地址出错！\n");
         exit(2);
    }
    printf("调用bind函数绑定套接字与地址成功！\n");
    ret=listen(sockfd,4);
    if (ret<0)
    {
        printf("调用listen函数出错，无法宣告服务器已经可以接受连接！\n");
         exit(3);
    }
    printf("调用listen函数成功，宣告服务器已经可以接受连接请求！\n");
    newfd=accept(sockfd,NULL,NULL);/*newfd连接到调用connect的客户端*/
    if (newfd<0)
    {
        printf("调用accept函数出错，无法接受连接请求，建立连接失败！\n");
         exit(4);
    }
    printf("调用accept函数成功，服务器与客户端建立连接成功！\n");
    while (1)
    {
        recv_num=recv(newfd,buf,24,0);
        if (recv_num<0)
            printf("调用recv接受失败！\n");
        else
        {
            recv_num_total+=recv_num;
            printf("调用recv函数成功，本次接受到%d个字节，内容为：\"%s\"。共受到%d个字节的数据。\n",recv_num,buf,recv_num_total);
        }
        sleep(2);
    }
}
