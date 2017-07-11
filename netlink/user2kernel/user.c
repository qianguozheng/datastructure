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

#define NETLINK_USER 22
#define USER_MSG    (NETLINK_USER + 1)
#define MSG_LEN 100

#define MAX_PLOAD 100

struct _my_msg
{
    struct nlmsghdr hdr;
    int8_t  data[MSG_LEN];
};



int main(int argc, char **argv)
{
    char *data = "hello kernel";
    struct sockaddr_nl  local, dest_addr;

    int skfd;
    struct nlmsghdr *nlh = NULL;
    struct _my_msg info;
    int ret;

    skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
    if(skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
        return -1;
    }

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

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // to kernel
    dest_addr.nl_groups = 1<<3 || 1 <<5;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = local.nl_pid; //self port

    memcpy(NLMSG_DATA(nlh), data, strlen(data));
    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));

    if(!ret)
    {
        perror("sendto error1\n");
        close(skfd);
        exit(-1);
    }
    printf("wait kernel msg!\n");
    memset(&info, 0, sizeof(info));
    ret = recvfrom(skfd, &info, sizeof(struct _my_msg), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if(!ret)
    {
        perror("recv form kernel error\n");
        close(skfd);
        exit(-1);
    }

    printf("msg receive from kernel:%s\n", info.data);
    close(skfd);

    free((void *)nlh);
    return 0;

}
