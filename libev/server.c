
// a single header file is required
#include <ev.h>

#include <stdio.h> // for puts
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <linux/if.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6/ip6_tables.h>

#include "buffer.h"

#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif

typedef struct server_ctx{
	ev_io io;
	int connected;
	struct server *server;
}server_ctx_t;

typedef struct server{
	int fd;
	buffer_t *buf;
	struct sockaddr_storage destaddr;
	struct server_ctx *recv_ctx;
	struct server_ctx *send_ctx;
}server_t;

static void accept_cb(EV_P_ ev_io *w, int revents);
static void server_recv_cb(EV_P_ ev_io *w, int revents);
static void server_send_cb(EV_P_ ev_io *w, int revents);
///static void remote_recv_cb(EV_P_ ev_io *w, int revents);
///static void remote_send_cb(EV_P_ ev_io *w, int revents);

static server_t *new_server(int fd)
{
	server_t *server;
	server = malloc(sizeof(server_t));
	
	server->recv_ctx = malloc(sizeof(server_ctx_t));
	server->send_ctx = malloc(sizeof(server_ctx_t));
	server->buf = malloc(sizeof(buffer_t));
	server->fd = fd;
	server->recv_ctx->connected = 0;
	server->recv_ctx->server = server;
	server->send_ctx->connected = 0;
	server->send_ctx->server = server;
	
	ev_io_init(&server->recv_ctx->io, server_recv_cb, fd, EV_READ);
	ev_io_init(&server->send_ctx->io, server_send_cb, fd, EV_WRITE);
	
	balloc(server->buf, BUF_SIZE);
	
	return server;
}

static void free_server(server_t *server)
{
	if (NULL != server){
		if (NULL != server->buf)
		{
			bfree(server->buf);
			free(server->buf);
		}
		free(server->recv_ctx);
		free(server->send_ctx);
		free(server);
		
	}
}

static void close_and_free_server(EV_P_ server_t *server)
{
	if (NULL != server)
	{
		ev_io_stop(EV_A_ & server->send_ctx->io);
		ev_io_stop(EV_A_ & server->recv_ctx->io);
		close(server->fd);
		free_server(server);
	}
}

int setnonblocking(int fd)
{
	int flags;
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
	{
		flags = 0;
	}
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int getdestaddr(int fd, struct sockaddr_storage *destaddr)
{
	socklen_t socklen = sizeof(*destaddr);
	int error = 0;
	
	error = getsockopt(fd, SOL_IPV6, IP6T_SO_ORIGINAL_DST, destaddr, &socklen);
	if (error){
		error = getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, destaddr, &socklen);
		if (error){
			return -1;
		}
	}
	return 0;
}

int set_reuseport (int socket)
{
	int opt = 1;
	return setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

int create_and_bind(const char *addr, const char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, listen_sock;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	s = getaddrinfo(addr, port, &hints, &result);
	if (0 != s)
	{
		printf("getaddrinfo failed");
		return -1;
	}
	
	for (rp = result; rp != NULL; rp=rp->ai_next)
	{
		listen_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (-1 == listen_sock)
		{
			continue;
		}
		
		int opt = 1;
		setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		#ifdef SO_NOSIGPIPE
		setsockopt(listen_sock, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
		#endif
		
		int err = set_reuseport(listen_sock);
		if (err == 0)
		{
			printf("tcp port reuse enabled\n");
		}
		else
		{
			printf("tcp port reuse failed\n");
		}
		
		s = bind(listen_sock, rp->ai_addr, rp->ai_addrlen);
		if (0 == s){
			printf("Bind successfully\n");
			break;
		}
		else
		{
			printf("bind error\n");
		}
		
		close(listen_sock);
	}
	if (NULL == rp)
	{
		return -1;
	}
	
	freeaddrinfo(result);
	
	return listen_sock;
}

//buffer_t data;
static void server_recv_cb(EV_P_ ev_io *w, int revents)
{
	//Buffer should be dynamic
	server_ctx_t *server_recv_ctx = (server_ctx_t *)w;
	server_t *server = server_recv_ctx->server;
	//char buf[BUF_SIZE];
	//memset(buf, 0, BUF_SIZE);
	
	printf("server_recv_cb serverfd=%d, %d\n", w->fd, server->fd);
	ssize_t r = recv(server->fd, server->buf->array + server->buf->len, BUF_SIZE, 0);
	if (0 == r)
	{
		perror("r==0\n");
		close_and_free_server(EV_A_ server);
	}
	else if (r < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			//no data
			//continue to wait for recvj
			return;
		}
		else
		{
			perror("server recv error\n");
			close_and_free_server(EV_A_ server);
			//???? close fd or someting else
			//close(w->fd);
			
		}
	}
	printf("server_recv_cb buf=%s\n", server->buf->array);
	server->buf->len = server->buf->len + r;
	// 此处需要组包
	if (r >= BUF_SIZE)
	{
		brealloc(server->buf, server->buf->len, server->buf->len + BUF_SIZE);
		return;
		//memmove();
	}
	//process content recevied from client.
	/* 判断是否为完整的协议数据报文，再决定是否处理 */
	
	//ssize_t s = send(w->fd, buf, strlen(buf), 0);
	char test[409600];
	//memset(test, 'X', sizeof(test));
	
	ssize_t s = send(server->fd, test, strlen(test), 0);
	if (s < 0){
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			perror("send error");
			close_and_free_server(EV_A_ server);
			//close(w->fd);
		}
		return;
	}
	else if (s < strlen(test))
	{
		//Buf need to move on send that unsent data;
		printf("send buf partly, wait for the next time to send\n");
		//close_and_free_server(EV_A_ server);
		//close(w->fd);
		return ;
	}
	else
	{
		//send event stoped
		
		//close(w->fd);
	}
	close_and_free_server(EV_A_ server);
}

static void server_send_cb(EV_P_ ev_io *w, int revents)
{
	printf("server send cb fd=%d\n", w->fd);
	
	ev_io_stop(EV_A_ w);
}

static void accept_cb(EV_P_ ev_io *w, int revents)
{
	struct sockaddr_storage destaddr;
	int err;
	
	struct sockaddr_in addr;
	int len = sizeof(struct sockaddr_in);
	
	printf("w->fd=%d\n", w->fd);
	int serverfd = accept(w->fd, (struct sockaddr*)&addr, &len);
	printf("accept_cb serverfd=%d\n", serverfd);
	if (-1 == serverfd)
	{
		perror("serverfd is error\n");
		
		return;
	}
	
	//err = getdestaddr(serverfd, &destaddr);
	//if (err){
	//	printf("getdestaddr error\n");
	//	return;
	//}
	
	setnonblocking(serverfd);
	
	int opt = 1;
	setsockopt(serverfd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt));
	#ifdef SO_NOSIGPIPE
	setsockopt(serverfd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
	#endif
	
	printf("accept_cb serverfd2=%d\n", serverfd);
	
	//ev_io rcv_io;
	//ev_io snd_io;
	server_t *server = new_server(serverfd);
	//ev_io_init(&rcv_io, server_recv_cb, serverfd, EV_READ);
    //ev_io_init(&snd_io, server_send_cb, serverfd, EV_WRITE);
	//ev_io_start(EV_A_ & rcv_io);
	ev_io_start(EV_A_ &server->recv_ctx->io);
}

int
main (void)
{
 
	int listenfd;
	ev_io io;
	struct ev_loop *loop = EV_DEFAULT;
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	
	listenfd = create_and_bind("127.0.0.1", "10001");
	
	setnonblocking(listenfd);
	
	if (listenfd < 0)
	{
		printf("listen sock error\n");
	}
	
	if (listen(listenfd, 128) == -1){
		printf("listen error\n");
	}
	
	ev_io_init(&io, accept_cb, listenfd, EV_READ);
	ev_io_start(loop, &io);
	
	ev_run(loop, 0);
	
 return 0;
}
