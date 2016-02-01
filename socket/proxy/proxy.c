#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "util.h"

#define SERVER_PORT 5900
int should_exit = 0;
int listenfd = 0;
//TODO: log/debug system.

static int create_socket(void){
	struct sockaddr_in server_addr;
	//int listenfd;
	
	/*if (unlink(socket_path) == -1 && errno != ENOENT)
	{
		printf("unlink (%s) failed: %s", socket_path, strerror(errno));
		return -1;
	}*/
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("socket init failed %s", strerror(errno));
		return -1;
	}
	
	int reuse = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) == -1)
	{
		printf("setsockopt SO_REUSEADDR failed\n");
		return -1;
	}
	
	int flags = fcntl(listenfd, F_GETFL, 0);
	if (flags < 0)
	{
		printf("Couldn't get the listen fd flags\n");
	}
	else 
	{
		if (fcntl(listenfd, F_SETFL, flags | O_NONBLOCK) < 0)
		{
			printf("Couldn't set socket to non-blocking");
		}
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind socket error\n");
		return -1;
	}
	
	if (listen(listenfd, 10) < 0)
	{
		printf("listen socket error\n");
		return -1;
	}
	
	return listenfd;
}

static int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
{
	int ready;
	sigset_t origmask;
	int to = timeout->tv_sec*1000 + timeout->tv_nsec/1000000;

	sigprocmask(SIG_SETMASK, sigmask, &origmask);
	ready = poll(fds, nfds, to);
	sigprocmask(SIG_SETMASK, &origmask, NULL);

	return ready;
}

//IO model of Server and Client
static int main_loop()
{
	//TODO: work as server, accept from client
	//TODO: work as client, request to server
	//TODO: server and client should be one to one.
	//TODO: transparent proxy support
	//TODO: caching desired file from server, as feed it to client when
	//		same request come again.
	
	int cnt, to, i;
	struct fdlist pollfds;
	struct timespec tspec;

	sigset_t empty_sigset;
	sigemptyset(&empty_sigset); // unmask all signals
	
	fdlist_create(&pollfds);
	
	to = 2;
	
	while(!should_exit)
	{
		tspec.tv_sec = to / 1000;
		tspec.tv_nsec = (to % 1000) * 1000000;
		
		fdlist_reset(&pollfds);
		fdlist_add(&pollfds, FD_LISTEN, listenfd, POLLIN);
		server_get_fds(&pollfds);
		client_get_fds(&pollfds);
		
		cnt = ppoll(pollfds.fds, pollfds.count, &tspec, &empty_sigset);
		
		if(cnt == -1) {
			if(errno == EINTR) {
				if(should_exit) {
					break;
				}
				//if(should_discover) {
				//	should_discover = 0;
					//usb_discover();
				//}
			}
			printf("poll failed: %s", strerror(errno));
		} else if(cnt == 0) {
			//if(usb_process() < 0) {
			//	fdlist_free(&pollfds);
			//	return -1;
			//}
			//device_check_timeouts();
			
			//printf("poll timeout: %s", strerror(errno));
		} else {
			//int done_usb = 0;
			for(i=0; i<pollfds.count; i++) {
				if(pollfds.fds[i].revents) {
					//if(/*!done_usb &&*/ pollfds.owners[i] == FD_SERVER) {
						
						//TODO: process server response
						//if(usb_process() < 0) {
							
						//	fdlist_free(&pollfds);
						//	return -1;
						//}
						//done_usb = 1;
					//}
					if(pollfds.owners[i] == FD_LISTEN) {
						
						//TODO: process client request
						printf("FD_LISTEN accepted\n");
						if(client_accept(listenfd) < 0) {
							
							fdlist_free(&pollfds);
							return -1;
						}
					}
					if(pollfds.owners[i] == FD_CLIENT) {
						//Read data from client
						client_process(pollfds.fds[i].fd, pollfds.fds[i].revents, FD_CLIENT);
					}
					if(pollfds.owners[i] == FD_SERVER) {
						//Build packet and send it to server, wait for response
						client_process(pollfds.fds[i].fd, pollfds.fds[i].revents, FD_SERVER);
					}
				}
			}
		}
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	//TODO: 1. file lock
	//TODO: 2. signal process
	//TODO: 3. possible some parameters, init
	create_socket();
	main_loop();
}
