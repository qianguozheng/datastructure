#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <poll.h>

#define PRINTF(a,...) printf(a "\n", ## __VA_ARGS__)

int init_netlink_socket(int * netlink_socket)
{
    struct sockaddr_nl snl;
    int ret;

    memset(&snl, 0, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    *netlink_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (*netlink_socket == -1)
    {
        PRINTF("netlink socket: %s\n", strerror(errno));
        return -1;
    }

    ret = bind(*netlink_socket, (struct sockaddr *) & snl,
               sizeof(struct sockaddr_nl));
    if (ret < 0)
    {
        PRINTF("netlink bind failed: %s\n", strerror(errno));
        close(*netlink_socket);
        *netlink_socket = -1;
        return -1;
    }

    return 0;
}

int main() 
{
    int ret = -1;
    int netlink_socket = -1;
    int buflen = -1;
    char netlink_buf[4096];
    struct pollfd fds[1];
    int nfds = 1;

    ret = init_netlink_socket(&netlink_socket);
    if (ret)
    {
        PRINTF("init_netlink_socket() has failed %s\n", strerror(errno));
        close(netlink_socket);
        return -1;
    }

    fds[0].fd = netlink_socket;
    fds[0].events  = POLLIN;
    fds[0].revents = 0;

    while (1)
    {
        ret = poll(fds, nfds, -1);
	if (ret <= 0)
	{
	    if (errno != EINTR)
	    {
	        PRINTF("poll() : %s\n", strerror(errno));
	    }
	    else /* Interrupted syscall(signal) */
	        continue;
        }
        else if(ret > 0)
	{
	    buflen = recv(netlink_socket, netlink_buf, sizeof(netlink_buf)-1 , 0);
            if (buflen <= 0)
            {
                PRINTF("error receiving uevent message: %s\n", strerror(errno));
            }
            else
            {
                netlink_buf[buflen-1] = '\0';
                printf("NETLINK => [%s]\n", netlink_buf);
	    }
            continue;
        }
    }

    return 0;
}
