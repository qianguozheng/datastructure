#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFLEN 255

int main ( int argc, char **argv )
{
    struct sockaddr_in peeraddr, localaddr;
    int sockfd;
    int socklen, n;
//(1)创建UDP数据报socket描述符
    sockfd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( sockfd<0 )
    {
        printf ( "socket creating err in udptalk\n" );
        exit ( EXIT_FAILURE );
    }
    printf ( "IP address Checking!\n" );
    socklen = sizeof ( struct sockaddr_in );
    memset ( &peeraddr, 0, socklen );
//(2)设置目标主机IP和端口，这里我们使用广播方式
    peeraddr.sin_family=AF_INET;
    peeraddr.sin_port=htons ( atoi ( "8903" ) );

    peeraddr.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
//(3设置本机IP和端口，这里我们设置可以接收符合端口的所有的包
    memset ( &localaddr, 0, socklen );
    localaddr.sin_family=AF_INET;
    localaddr.sin_addr.s_addr = htonl ( INADDR_ANY );  //设置接收任何主机

    printf ( "try to bind local address \n" );
    localaddr.sin_port=htons ( atoi ( "8904" ) );

//(4)设置IPPROTO_IP标志，以便获取UDP包中的信息
    int opt = 1;
    setsockopt ( sockfd, IPPROTO_IP, IP_PKTINFO, &opt, sizeof ( opt ) );
    int nb = 0;
//(5)设置为广播方式
    nb = setsockopt ( sockfd, SOL_SOCKET, SO_BROADCAST, ( char * ) &opt,
                      sizeof ( opt ) );
    if ( nb == -1 )
    {
        printf ( "set socket error..." );
        exit ( EXIT_FAILURE );
    }
    printf ( "IP address Checking!\n" );

    char cmbuf[100];// 这里只是为控制数据申请一个空间
//(6)初始化msg_iovec字段,以便获取UDP包数据域
    char buffer[BUFLEN+1];
    struct iovec iov[1];
    iov[0].iov_base=buffer;
    iov[0].iov_len=sizeof ( buffer );
//(7)初始化struct msghdr,以便获取UDP包中目标IP地址和源地址
    struct msghdr mh =
    {
        .msg_name = &localaddr,
        .msg_namelen = sizeof ( localaddr ),
        .msg_control = cmbuf,
        .msg_controllen = sizeof (cmbuf ),
        .msg_iov=iov,                                                           
        .msg_iovlen=1
    };
//(8)将本机的地址信息与sockfd绑定起来
    if ( bind ( sockfd, &localaddr, socklen ) <0 )
    {
        printf ( "bind local address err in udptalk!\n" );
        exit ( 2 );
    }
    //发一个消息给目标主机
    if ( sendto ( sockfd, "HELLO", strlen ( "HELLO" ), 0, &peeraddr, socklen )
<0 )
    {
        printf ( "sendto err in udptalk!\n" );
        exit ( 3 );
    }
    printf ( "end of sendto \n" );
    printf ( "start of recv&send message loop!\n" );
    for ( ;; )//接收消息循环
    {
        
        printf ( "Waiting For Message...!\n" );
        n=recvmsg ( sockfd, &mh, 0 );
        //判断socket是否有错误发生
        if ( n<0 )
        {
            printf ( "recvfrom err in udptalk!\n" );
            exit ( 4 );
        }
        else
        {
            cmbuf[n]=0;
            printf ( "Receive:%dByte。\tThe Message Is:%s\n", n,buffer );
        }
//(9)初始化cmsghdr以便处理mh中的附属数据，通过遍历附属数据对象,找出我们感兴趣的信息
        struct cmsghdr *cmsg ;
        for ( cmsg = CMSG_FIRSTHDR ( &mh );
                cmsg != NULL;
                cmsg = CMSG_NXTHDR ( &mh, cmsg ) )
        {
         // 忽略我们不需要的控制头（the control headers）
            if ( cmsg->cmsg_level != IPPROTO_IP ||
                    cmsg->cmsg_type != IP_PKTINFO )
            {
                continue;
            }
            struct in_pktinfo *pi = CMSG_DATA ( cmsg );    
           
//(10)将地址信息转换后输出
            char dst[100],ipi[100];//用来保存转化后的源IP地址，目标主机地址   
            // pi->ipi_spec_dst 是UDP包中的路由目的IP地址（the destination in_addr）
            // pi->ipi_addr 是UDP包中的头标识目的地址（the receiving interface in_addr）
            if ( ( inet_ntop ( AF_INET,& ( pi->ipi_spec_dst ),dst,sizeof ( dst )
                             ) ) !=NULL )
            {
                printf ( "路由目的IP地址IPdst=%s\n",dst);
            }
            if ( ( inet_ntop ( AF_INET,& ( pi->ipi_addr ),ipi,sizeof ( ipi )
                             ) ) !=NULL )
            {
                printf ("头标识目的地址ipi_addr=%s\n",ipi);
            }

        }

        printf ( "Send Some Message To Server\n" );
        if ( sendto ( sockfd, "Hello", strlen ( buffer ), 0, &peeraddr, socklen)
<0 )
        {
            printf ( "sendto err in udptalk!\n" );
            exit ( 3 );
        }
    }
}
