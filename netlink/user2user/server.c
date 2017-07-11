/*******************************
file:           u_netlink.c
description:    netlink demo
author:         arvik
email:          1216601195@qq.com
blog:           http://blog.csdn.net/u012819339
*******************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#define NLINK_MSG_LEN 1024


int main(int argc, char **argv)
{
    char *data = "hello 51\n";
    struct sockaddr_nl  local, dest_addr;
    int skfd;

    skfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    if(skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
        return -1;
    }

	//Bind local address
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = 51; 
    local.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
    {
        printf("bind() error\n");
        close(skfd);
        return -1;
    }
    
    struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(NLINK_MSG_LEN));
#if 0
    nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);
    nlh->nlmsg_pid = 1;
    nlh->nlmsg_flags = 0;
    
    strcpy(NLMSG_DATA(nlh), "Hello 51\n");

    //Send data
    struct iovec iov;
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    
    struct msghdr msg;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    sendmsg(skfd, &msg, 0);
#endif
	
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
    memset(nlh, 0, NLMSG_SPACE(NLINK_MSG_LEN));
    
    struct iovec iov;
    char buf[NLINK_MSG_LEN];
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    int len = recvmsg(skfd, &msg, 0);
    if (len <= 0){
		printf("no message\n");
		return 0;
	}
	printf("receive length=%d, %s\n", len, strerror(errno));
    nlh = (struct nlmsghdr *)msg.msg_iov[0].iov_base;
    
    printf("%s\n", NLMSG_DATA(nlh));
    
    close(skfd);
    return 0;
}
