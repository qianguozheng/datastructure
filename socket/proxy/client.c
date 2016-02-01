#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>

#include <assert.h>
#include "client.h"
#include "util.h"

#define CMD_BUF_SIZE	0x10000
#define REPLY_BUF_SIZE	0x10000

enum client_state {
	CLIENT_COMMAND,		// waiting for command
	CLIENT_LISTEN,		// listening for devices
	CLIENT_CONNECTING1,	// issued connection request
	CLIENT_CONNECTING2,	// connection established, but waiting for response message to get sent
	CLIENT_CONNECTED,	// connected
	CLIENT_DEAD
};

struct mux_client {
	int fd;
	unsigned char *ob_buf;
	uint32_t ob_size;
	uint32_t ob_capacity;
	unsigned char *ib_buf;
	uint32_t ib_size;
	uint32_t ib_capacity;
	short events, devents;
	uint32_t connect_tag;
	//int connect_device;
	enum client_state state;
	uint32_t proto_version;
	//Weeds Add
	int server_fd;
	
};


static struct collection client_list;
pthread_mutex_t client_list_mutex;



/**
 * Receive raw data from the client socket.
 *
 * @param client Client to read from.
 * @param buffer Buffer to store incoming data.
 * @param len Max number of bytes to read.
 * @return Same as recv() system call. Number of bytes read; when < 0 errno will be set.
 */
int client_read(struct mux_client *client, void *buffer, uint32_t len)
{
	printf( "client_read fd %d buf %p len %d\n", client->fd, buffer, len);
	if(client->state != CLIENT_CONNECTED) {
		printf("Attempted to read from client %d not in CONNECTED state", client->fd);
		return -1;
	}
	return recv(client->fd, buffer, len, 0);
}

/**
 * Send raw data to the client socket.
 *
 * @param client Client to send to.
 * @param buffer The data to send.
 * @param len Number of bytes to write.
 * @return Same as system call send(). Number of bytes written; when < 0 errno will be set.
 */
int client_write(struct mux_client *client, void *buffer, uint32_t len)
{
	int sret = -1;

	printf("client_write fd %d buf %p len %d", client->fd, buffer, len);
	if(client->state != CLIENT_CONNECTED) {
		printf( "Attempted to write to client %d not in CONNECTED state", client->fd);
		return -1;
	}

	sret = send(client->fd, buffer, len, 0);
	if (sret < 0) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			printf( "ERROR: client_write: fd %d not ready for writing", client->fd);
		} else {
			printf( "ERROR: client_write: sending to fd %d failed: %s", client->fd, strerror(errno));
		}
	}
	return sret;
}

/**
 * Set event mask to use for ppoll()ing the client socket.
 * Typically POLLOUT and/or POLLIN. Note that this overrides
 * the current mask, that is, it is not ORing the argument
 * into the current mask.
 *
 * @param client The client to set the event mask on.
 * @param events The event mask to sert.
 * @return 0 on success, -1 on error.
 */
int client_set_events(struct mux_client *client, short events)
{
	if((client->state != CLIENT_CONNECTED) && (client->state != CLIENT_CONNECTING2)) {
		printf("client_set_events to client %d not in CONNECTED state", client->fd);
		return -1;
	}
	client->devents = events;
	if(client->state == CLIENT_CONNECTED)
		client->events = events;
	return 0;
}

/**
 * Wait for an inbound connection on the usbmuxd socket
 * and create a new mux_client instance for it, and store
 * the client in the client list.
 *
 * @param listenfd the socket fd to accept() on.
 * @return The connection fd for the client, or < 0 for error
 *   in which case errno will be set.
 */
int client_accept(int listenfd)
{
	struct sockaddr addr;
	int cfd;
	socklen_t len = sizeof(struct sockaddr);
	cfd = accept(listenfd, (struct sockaddr *)&addr, &len);
	if (cfd < 0) {
		printf("accept() failed (%s)", strerror(errno));
		return cfd;
	}

	int flags = fcntl(cfd, F_GETFL, 0);
	if (flags < 0) {
		printf("ERROR: Could not get socket flags!");
	} else {
		if (fcntl(cfd, F_SETFL, flags | O_NONBLOCK) < 0) {
			printf( "ERROR: Could not set socket to non-blocking mode");
		}
	}
	
	struct mux_client *client;
	client = malloc(sizeof(struct mux_client));
	memset(client, 0, sizeof(struct mux_client));

	client->fd = cfd;
	client->ob_buf = malloc(REPLY_BUF_SIZE);
	client->ob_size = 0;
	client->ob_capacity = REPLY_BUF_SIZE;
	client->ib_buf = malloc(CMD_BUF_SIZE);
	client->ib_size = 0;
	client->ib_capacity = CMD_BUF_SIZE;
	client->state = CLIENT_CONNECTED;
	client->events = POLLIN;

	pthread_mutex_lock(&client_list_mutex);
	collection_add(&client_list, client);
	pthread_mutex_unlock(&client_list_mutex);
	
	
	//client_read(client, client->ib_buf, CMD_BUF_SIZE);
	//printf("Client: %s\n", client->ib_buf);

#if 0
//#ifdef SO_PEERCRED
	if (log_level >= LL_INFO) {
		struct ucred cr;
		len = sizeof(struct ucred);
		getsockopt(cfd, SOL_SOCKET, SO_PEERCRED, &cr, &len);

		if (getpid() == cr.pid) {
			printf("New client on fd %d (self)", client->fd);
		} else {
			printf("New client on fd %d (pid %d)", client->fd, cr.pid);
		}
	}
#else
	printf( "New client on fd %d", client->fd);
#endif

	return client->fd;
}

void client_close(struct mux_client *client)
{
	printf("Disconnecting client fd %d", client->fd);
	if(client->state == CLIENT_CONNECTING1 || client->state == CLIENT_CONNECTING2) {
		//printf("Client died mid-connect, aborting device %d connection", client->connect_device);
		client->state = CLIENT_DEAD;
		//device_abort_connect(client->connect_device, client);
	}
	close(client->fd);
	if(client->ob_buf)
		free(client->ob_buf);
	if(client->ib_buf)
		free(client->ib_buf);
	pthread_mutex_lock(&client_list_mutex);
	collection_remove(&client_list, client);
	pthread_mutex_unlock(&client_list_mutex);
	free(client);
}

void client_get_fds(struct fdlist *list)
{
	pthread_mutex_lock(&client_list_mutex);
	FOREACH(struct mux_client *client, &client_list) {
		fdlist_add(list, FD_CLIENT, client->fd, client->events);
	} ENDFOREACH
	pthread_mutex_unlock(&client_list_mutex);
}


void server_get_fds(struct fdlist *list)
{
	pthread_mutex_lock(&client_list_mutex);
	FOREACH(struct mux_client *client, &client_list) {
		if (client->server_fd)
		{
			fdlist_add(list, FD_SERVER, client->server_fd, client->events);
		}
	} ENDFOREACH
	pthread_mutex_unlock(&client_list_mutex);
}

static void process_send(struct mux_client *client)
{
	int res;
	if(!client->ob_size) {
		printf("Client %d OUT process but nothing to send?", client->fd);
		client->events &= ~POLLOUT;
		return;
	}
	res = send(client->fd, client->ob_buf, client->ob_size, 0);
	if(res <= 0) {
		printf("Send to client fd %d failed: %d %s", client->fd, res, strerror(errno));
		client_close(client);
		return;
	}
	if((uint32_t)res == client->ob_size) {
		client->ob_size = 0;
		client->events &= ~POLLOUT;
		if(client->state == CLIENT_CONNECTING2) {
			printf("Client %d switching to CONNECTED state", client->fd);
			client->state = CLIENT_CONNECTED;
			client->events = client->devents;
			// no longer need this
			free(client->ob_buf);
			client->ob_buf = NULL;
		}
	} else {
		client->ob_size -= res;
		memmove(client->ob_buf, client->ob_buf + res, client->ob_size);
	}
}

char *parse_header_line(char *buffer)
{
	char *tmp = NULL, *end = NULL;
	//assert(buffer != NULL);
	printf("buffer=%s\n", buffer);
	if (buffer == NULL || (strlen(buffer) <= 4))
	{
		return NULL;
	}
	printf("=================\n");
	if ((tmp = strstr(buffer, "Host: "))!= NULL)
	{
		tmp += strlen("Host: ");
		end = strstr(tmp, "\r\n");
		printf("tmp=%s\n", tmp);
		if (end)
		{
			*end = '\0';
			printf("tmp=%s\n", tmp);
			return strdup(tmp);
		}
	}
	printf("$$$$$$$$$$$$$$$$\n");
	return NULL;
}

static void process_recv(struct mux_client *client)
{	
	client_read(client, client->ib_buf, CMD_BUF_SIZE);
	printf("Client: %s\n", client->ib_buf);
	
	//Parse Request First Line and Host.
	char *host = NULL;
	host = parse_header_line(client->ib_buf);
	printf("Host=%s\n", host);
	
	//Build Server Request to server, add socket into trunk.
	client->server_fd = opensock(host, 80, "192.168.151.251");
	
	if (host)
	{
		free(host);
	}
	
	if (client->server_fd)
	{
		int len = 0;
		len = send(client->server_fd, client->ib_buf, strlen(client->ib_buf), 0);
		printf("send len=%d\n", len);
	}
}
//static void process_recv(struct mux_client *client)
static void recv_server(struct mux_client *client)
{
	int len;
	
	len = recv(client->server_fd, client->ob_buf, CMD_BUF_SIZE, 0);
	printf("Len=%d, buf=[%s]\n", len, client->ob_buf);
	if (len > 0)
	{
		len = send(client->fd, client->ob_buf, strlen(client->ob_buf), 0);
		printf("send len=%d\n", len);
	}
}

void client_process(int fd, short events, enum fdowner owner)
{
	struct mux_client *client = NULL;
	pthread_mutex_lock(&client_list_mutex);
	FOREACH(struct mux_client *lc, &client_list) {
		if(lc->fd == fd) {
			client = lc;
			break;
		}
	} ENDFOREACH
	pthread_mutex_unlock(&client_list_mutex);

	if(!client) {
		printf("client_process: fd %d not found in client list", fd);
		return;
	}

	//if(client->state == CLIENT_CONNECTED) {
	//	printf("client_process in CONNECTED state");
		//device_client_process(client->connect_device, client, events);
	//} else 
	if (owner == FD_CLIENT)
	{
		if(events & POLLIN) {
			process_recv(client);
		} else if(events & POLLOUT) { //not both in case client died as part of process_recv
			process_send(client);
		}
	}
	else if (FD_SERVER == owner)
	{
		if(events & POLLIN) {
			//process_recv(client);
			recv_server(client);
		} else if(events & POLLOUT) { //not both in case client died as part of process_recv
			//process_send(client);
		}
	}
}

/*void client_device_add(struct device_info *dev)
{
	pthread_mutex_lock(&client_list_mutex);
	printf "client_device_add: id %d, location 0x%x, serial %s", dev->id, dev->location, dev->serial);
	device_set_visible(dev->id);
	FOREACH(struct mux_client *client, &client_list) {
		if(client->state == CLIENT_LISTEN)
			notify_device_add(client, dev);
	} ENDFOREACH
	pthread_mutex_unlock(&client_list_mutex);
}

void client_device_remove(int device_id)
{
	pthread_mutex_lock(&client_list_mutex);
	uint32_t id = device_id;
	printf "client_device_remove: id %d", device_id);
	FOREACH(struct mux_client *client, &client_list) {
		if(client->state == CLIENT_LISTEN)
			notify_device_remove(client, id);
	} ENDFOREACH
	pthread_mutex_unlock(&client_list_mutex);
}*/

/*
 * Bind the given socket to the supplied address.  The socket is
 * returned if the bind succeeded.  Otherwise, -1 is returned
 * to indicate an error.
 */
static int
bind_socket (int sockfd, const char *addr, int family)
{
        struct addrinfo hints, *res, *ressave;

        assert (sockfd >= 0);
        assert (addr != NULL && strlen (addr) != 0);

        memset (&hints, 0, sizeof (struct addrinfo));
        hints.ai_family = family;
        hints.ai_socktype = SOCK_STREAM;

        /* The local port it not important */
        if (getaddrinfo (addr, NULL, &hints, &res) != 0)
                return -1;

        ressave = res;

        /* Loop through the addresses and try to bind to each */
        do {
                if (bind (sockfd, res->ai_addr, res->ai_addrlen) == 0)
                        break;  /* success */
        } while ((res = res->ai_next) != NULL);

        freeaddrinfo (ressave);
        if (res == NULL)        /* was not able to bind to any address */
                return -1;

        return sockfd;
}

/*
 * Open a connection to a remote host.  It's been re-written to use
 * the getaddrinfo() library function, which allows for a protocol
 * independent implementation (mostly for IPv4 and IPv6 addresses.)
 */
int opensock (const char *host, int port, const char *bind_to)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;
	char portstr[6];

	assert (host != NULL);
	assert (port > 0);

	printf("opensock: opening connection to %s:%d\n", host, port);

	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	snprintf (portstr, sizeof (portstr), "%d", port);

	n = getaddrinfo (host, portstr, &hints, &res);
	if (n != 0) {
			printf("opensock: Could not retrieve info for %s\n", host);
			return -1;
	}

	printf("opensock: getaddrinfo returned for %s:%d\n", host, port);

	ressave = res;
	do {
			sockfd =
				socket (res->ai_family, res->ai_socktype, res->ai_protocol);
			if (sockfd < 0)
					continue;       /* ignore this one */

			/* Bind to the specified address */
			//if (bind_to) {
					if (bind_socket (sockfd, bind_to,
									 res->ai_family) < 0) {
							close (sockfd);
							continue;       /* can't bind, so try again */
					}
			//} else if (config.bind_address) {
			//		if (bind_socket (sockfd, config.bind_address,
			//						 res->ai_family) < 0) {
			//				close (sockfd);
			//				continue;       /* can't bind, so try again */
			//		}
			//}

			if (connect (sockfd, res->ai_addr, res->ai_addrlen) == 0)
					break;  /* success */

			close (sockfd);
	} while ((res = res->ai_next) != NULL);

	freeaddrinfo (ressave);
	if (res == NULL) {
			printf("opensock: Could not establish a connection to %s\n",
						 host);
			return -1;
	}

	return sockfd;
}


void client_init(void)
{
	printf ("client_init");
	collection_init(&client_list);
	pthread_mutex_init(&client_list_mutex, NULL);
}

void client_shutdown(void)
{
	printf("client_shutdown");
	FOREACH(struct mux_client *client, &client_list) {
		client_close(client);
	} ENDFOREACH
	pthread_mutex_destroy(&client_list_mutex);
	collection_free(&client_list);
}
