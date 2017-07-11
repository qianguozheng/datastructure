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
    int ret;

    skfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    if(skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
        return -1;
    }

	//Bind local address
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = 50; 
    local.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
    {
        printf("bind() error\n");
        close(skfd);
        return -1;
    }
    
    //Assign target peer address
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 51;
    dest_addr.nl_groups = 0;
    
    struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(NLINK_MSG_LEN));
    nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);
    nlh->nlmsg_pid = 50;
    nlh->nlmsg_flags = 0;
    
    strcpy(NLMSG_DATA(nlh), "Hello 51\n");
    
    //Send data
    struct iovec iov;
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
     int len = sendmsg(skfd, &msg, 0);
     printf("send len=%d, error=%s\n", len, strerror(errno));
    close(skfd);
    return 0;
}
