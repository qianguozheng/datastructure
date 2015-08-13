/* 实现功能：通过poll, 处理多个socket
 * 监听一个端口,监听到有链接时,添加到poll.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/time.h>
#include <netinet/in.h>

typedef struct _CLIENT{
    int fd;
    struct sockaddr_in addr; /* client's address information */
} CLIENT;

#define MYPORT 59000

//最多处理的connect
#define BACKLOG 5

//当前的连接数
int currentClient = 0; 

//数据接受 buf
#define REVLEN 10
char recvBuf[REVLEN];

#define OPEN_MAX 1024

int main()
{
    int i, ret, sinSize;
    int recvLen = 0;
    fd_set readfds, writefds;
    int sockListen, sockSvr, sockMax;
    int timeout;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    struct pollfd clientfd[OPEN_MAX];


    //socket
    if((sockListen=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\n");
        return -1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family  =  AF_INET;
    server_addr.sin_port = htons(MYPORT);
    server_addr.sin_addr.s_addr  =  htonl(INADDR_ANY); 

    //bind
    if(bind(sockListen, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("bind error\n");
        return -1;
    }

    //listen
    if(listen(sockListen, 5) < 0)
    {
        printf("listen error\n");
        return -1;
    }


    //clientfd 初始化
    clientfd[0].fd = sockListen;
    clientfd[0].events = POLLIN; //POLLRDNORM;
    sockMax = 0;
    for(i=1; i<OPEN_MAX; i++)
    {
        clientfd[i].fd = -1;
    }

    //select
    while(1)
    {
        timeout=3000;                
        //select
        ret = poll(clientfd, sockMax+1, timeout);
    
        if(ret < 0)
        {
            printf("select error\n");
            break;
        }
        else if(ret == 0)
        {
            printf("timeout ...\n");
            continue;
        }
    
        if (clientfd[0].revents & POLLIN)//POLLRDNORM
        {
            sockSvr = accept(sockListen, NULL, NULL);//(struct sockaddr*)&client_addr
        
            if(sockSvr == -1)
            {
                printf("accpet error\n");
            }
            else
            {
                currentClient++;
            }
        
            for(i=0; i<OPEN_MAX; i++)
            {
                if(clientfd[i].fd<0)
                {
                    clientfd[i].fd = sockSvr;
                    break;
                }
            }
            if(i==OPEN_MAX)
            {
                printf("too many connects\n");
                return -1;
            }
            clientfd[i].events = POLLIN;//POLLRDNORM;
            if(i>sockMax)
                sockMax = i;
        }
    
        //读取数据
        for(i=1; i<=sockMax; i++)
        {
            if(clientfd[i].fd < 0)
                continue;
        
            if (clientfd[i].revents & (POLLIN | POLLERR))//POLLRDNORM
            {
                if(recvLen != REVLEN)
                {
                    while(1)
                    {
                        //recv数据
                        ret = recv(clientfd[i].fd, (char *)recvBuf+recvLen, REVLEN-recvLen, 0);
                        if(ret == 0)
                        {
                            clientfd[i].fd = -1;
                            recvLen = 0;
                            break;
                        }
                        else if(ret < 0)
                        {
                            clientfd[i].fd = -1;
                            recvLen = 0;
                            break;
                        }
                        //数据接受正常
                        recvLen = recvLen+ret;
                        if(recvLen<REVLEN)
                        {
                            continue;
                        }
                        else
                        {
                            //数据接受完毕
                            printf("buf = %s\n",  recvBuf);
                            //close(client[i].fd);
                            //client[i].fd = -1;
                            recvLen = 0;
                            break;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
