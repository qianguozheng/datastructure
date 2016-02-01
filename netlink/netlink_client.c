#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include <poll.h>

#define NETLINK_TEST 25
#define MAX_PAYLOAD 1024 // maximum payload size

int main(int argc, char* argv[])
{
    struct sockaddr_nl addr;
    int sz = 64 * 1024; // 64K buffer
    int fd;
    
	struct pollfd fds;
    char buffer[1024 + 2]; // max message length is 1024
    int count, nr;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = 0xffffffff;

    fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if(fd < 0)
    {
        printf("create an endpoint for UEVENT communication failed. error: %s", strerror(errno));
        return -1;
    }

    setsockopt(fd, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz));

    if(bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        printf("bind UEVENT socket failed. error: %s",strerror(errno));
        close(fd);
        return -1;
    }

    for (;;)
    {
        fds.fd = fd;
        fds.events = POLLIN;
        fds.revents = 0;

        nr = poll(&fds, 1, -1);

        if(nr > 0 && fds.revents == POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
       //     qxtLog->info() << QString::fromUtf8("等待读取UEvent。。。");
            count = recv(fd, buffer, sizeof(buffer), 0);
            
            printf("UEVEN buffer=%s\n", buffer);
            
#if 0
            if (count > 0)
            {
//RECV_BUFFER:
                //替换掉UEvent中的不可见字符，方便打印调试
                int i = 0;
                for (i = 1; i < 1026; i++)
                {
                    if (buffer[i-1] == '\n' && buffer[i] == '\0')
                    {
                        /*
                         * REMIND:这是一个糟糕的编码，因为在桌面环境下，会收到两次UEvent,
                         * 一次是系统的，一次是用户态的。
                         * 我们需要的是用户态的。
                         * 而用户态的UEvent buffer中的内容是 libudev......\0\0\0\0.....，
                         * 为了方便调试，将UEvent打印出来，所以这里硬编码一个40用来将用户态
                         * UEvent前面的\0\0\0\0...去掉。
                         **/
                        if (i < 40)
                            buffer[i] = ' ';
                        else
                            break;
                    }

                    else if (buffer[i] == '\0')
                        buffer[i] = '\n';

                    else if (!isprint(buffer[i]))
                        buffer[i] = ' ';
                }

                //QString uEventString(buffer);

//                qxtLog->debug() << QString("receive uevent:\n%1")
//                                   .arg(uEventString);

#if __x86_64
                /*
                 * 目前发现在桌面环境（Ubuntu 14.04 64bit）下会有两次UEvent，一次是内核的，一次是用户态的。
                 * 这部分代码可以过滤掉内核的事件，可以避免usb设备被系统占用的bug。
                 * 仅在桌面环境下有效。
                 * */
                if (!uEventString.contains("libudev"))
                {
                    qxtLog->debug() << QString::fromUtf8("KERNEL UEvent，忽略");
                    continue;
                }
#endif

				printf("UEVEN buffer=%s\n", buffer);

            }
#endif
        }
    }

    close(fd);

    return 0;
}
