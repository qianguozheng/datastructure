#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
    int count = 0;
    int listenfd, socketfd;
    int nread;
    struct sockaddr_in servaddr;
    struct timeval timeoutval;
    char readbuf[256];

    printf("accept started\n");

    //socket    
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket error\n");
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(59000);

    //bind
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error\n");
        //return -1;
    }

    //listen
    listen(listenfd, 5);

    //accept
    socketfd = accept(listenfd, NULL, NULL);



    while(1)
    {
        printf("start receive %d...\n", count++);
        memset(readbuf, sizeof(readbuf), 0);

        nread = recv(socketfd, readbuf, 10, 0);
        if(nread>0)
        {
            readbuf[10] = '\0';
            printf("receiveed %s, nread = %d\n\n", readbuf, nread);
        }
    }

    return 0;
}
