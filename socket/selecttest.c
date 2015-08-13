/* 实现功能：通过select处理多个socket
 * 监听一个端口,监听到有链接时,添加到select的w.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>

typedef struct _CLIENT{
    int fd;
    struct sockaddr_in addr; /* client's address information */
} CLIENT;

#define MYPORT 59000

//最多处理的connect
#define BACKLOG 5

//最多处理的connect
CLIENT client[BACKLOG];

//当前的连接数
int currentClient = 0; 

//数据接受 buf
#define REVLEN 10
char recvBuf[REVLEN];
//显示当前的connection
void showClient();

int main()
{
    int i, ret, sinSize;
    int recvLen = 0;
    fd_set readfds, writefds;
    int sockListen, sockSvr, sockMax;
    struct timeval timeout;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    for(i=0; i<BACKLOG; i++)
    {
        client[i].fd = -1;
    }

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

    for(i=0; i<BACKLOG; i++)
    {
        client[i].fd = -1;
    }

    //select
    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockListen, &readfds);
        sockMax = sockListen;
    
        //加入client
        for(i=0; i<BACKLOG; i++)
        {
            if(client[i].fd >0)
            {
                FD_SET(client[i].fd, &readfds);
                if(sockMax<client[i].fd) 
                    sockMax = client[i].fd;
            }
        }
        
        timeout.tv_sec=3;                
        timeout.tv_usec=0;
        //select
        ret = select((int)sockMax+1, &readfds, NULL, NULL, &timeout);
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
        printf("test111\n");
    
        //读取数据
        for(i=0; i<BACKLOG; i++)
        {
            if(client[i].fd>0 && FD_ISSET(client[i].fd, &readfds))
            {
                if(recvLen != REVLEN)
                {
                    while(1)
                    {
                        //recv数据
                        ret = recv(client[i].fd, (char *)recvBuf+recvLen, REVLEN-recvLen, 0);
                        if(ret == 0)
                        {
                            client[i].fd = -1;
                            recvLen = 0;
                            break;
                        }
                        else if(ret < 0)
                        {
                            client[i].fd = -1;
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
                            printf("%s, buf = %s\n", inet_ntoa(client[i].addr.sin_addr) , recvBuf);
                            //close(client[i].fd);
                            //client[i].fd = -1;
                            recvLen = 0;
                            break;
                        }
                    }
                }
            }
        }
    
        //如果可读
        if(FD_ISSET(sockListen, &readfds))
        {
            printf("isset\n");
            sockSvr = accept(sockListen, NULL, NULL);//(struct sockaddr*)&client_addr
        
            if(sockSvr == -1)
            {
                printf("accpet error\n");
            }
            else
            {
                currentClient++;
            }
        
            for(i=0; i<BACKLOG; i++)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = sockSvr;
                    client[i].addr = client_addr;
                    printf("You got a connection from %s \n",inet_ntoa(client[i].addr.sin_addr) );
                    break;
                }
            }
            //close(sockListen);
        }
    }

    printf("test\n");
    return 0;
}

//显示当前的connection
void showClient()
{
    int i;
    printf("client count = %d\n", currentClient);

    for(i=0; i<BACKLOG; i++)
    {
        printf("[%d] = %d", i, client[i].fd);
    }
    printf("\n");
}
