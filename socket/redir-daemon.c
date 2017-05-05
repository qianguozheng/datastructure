/*
 *  A simple example daemon for netfilter/iptables REDIRECT based
 *  transparent proxies
 *
 *  Also a nice demo for simple tcp client/server apps.
 *
 *  Copyright (C) 2008  RIEGL Research ForschungsGmbH
 *  Copyright (C) 2008  Clifford Wolf <clifford@clifford.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *  
 */

// http://svn.clifford.at/tools/trunk/examples/redir-daemon.c

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/netfilter_ipv4.h>

// This is to not confuse the VIM syntax highlighting
#define CHECK_VAL_OPEN (
#define CHECK_VAL_CLOSE )

#define CHECK(result, check)                                                            \
        CHECK_VAL_OPEN{                                                                 \
                typeof(result) _R = (result);                                           \
                if (!(_R check)) {                                                      \
                        fprintf(stderr, "Error from '%s' (%d %s) in %s:%d.\n",          \
                                        #result, (int)_R, #check, __FILE__, __LINE__);  \
                        fprintf(stderr, "ERRNO(%d): %s\n", errno, strerror(errno));     \
                        abort();                                                        \
                }                                                                       \
                _R;                                                                     \
        }CHECK_VAL_CLOSE

void tcp_tunnel(int fd1, int port)
{
        int fd2 = CHECK(socket(AF_INET, SOCK_STREAM, 0), != -1);

        struct sockaddr_in servaddr;
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        CHECK(inet_pton(AF_INET, "127.0.0.1", (struct sockaddr*)&servaddr.sin_addr), != -1);
        servaddr.sin_port = htons(port);
        CHECK(connect(fd2, (struct sockaddr*)&servaddr, sizeof(servaddr)), != -1);

        while (1)
        {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(fd1, &rfds);
                FD_SET(fd2, &rfds);

                int fd_max = fd1 > fd2 ? fd1 : fd2;

                CHECK(select(fd_max+1, &rfds, NULL, NULL, NULL), != -1);

                if (FD_ISSET(fd1, &rfds)) {
                        char buf[512];
                        int r = CHECK(read(fd1, buf, 512), > 0);
                        int w = 0;
                        while (w < r)
                                w += CHECK(write(fd2, buf+w, r-w), > 0);
                }

                if (FD_ISSET(fd2, &rfds)) {
                        char buf[512];
                        int r = CHECK(read(fd2, buf, 512), > 0);
                        int w = 0;
                        while (w < r)
                                w += CHECK(write(fd1, buf+w, r-w), > 0);
                }
        }
}

int main()
{
        struct sockaddr_in addr;
        int listenfd = CHECK(socket(AF_INET, SOCK_STREAM, 0), != -1);

        bzero(&addr,sizeof(addr));
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(666);

        int on = 1;
        CHECK(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, (socklen_t)sizeof(on)), != -1);

        struct linger sl = { 1, 5 };
        CHECK(setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &sl, (socklen_t)sizeof(sl)), != -1);

        CHECK(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)), != -1);
        CHECK(listen(listenfd, 5), != -1);

        CHECK(signal(SIGCHLD,SIG_IGN), != SIG_ERR);

        while (1) {
                int fd = CHECK(accept(listenfd,NULL, NULL), != -1);
                if (!fork()) {
                        struct sockaddr_in trgaddr;
                        size_t trgaddr_len = sizeof(trgaddr);
                        CHECK(getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, (struct sockaddr*)&trgaddr, &trgaddr_len), != -1);
                        printf("Connection to port %d.\n", ntohs(trgaddr.sin_port));
                        tcp_tunnel(fd, ntohs(trgaddr.sin_port));
                        return 0;
                } else
                        close(fd);
        }

        return 0;
}
