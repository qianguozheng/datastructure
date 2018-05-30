#include <event.h>
#include <evhttp.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "dhcpinfo.h"

int httpserver_bindsocket(int port, int backlog);
int httpserver_start(int port, int nthreads, int backlog);
void *httpserver_dispatch(void *arg);
void httpserver_generic_handler(struct evhttp_request *req, void *arg);
void httpserver_process_request(struct evhttp_request *req);
static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 0, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}

static void _buffer_clean(const void *data, size_t datalen, void *extra)
{
	free((void *)data);
}

int httpserver_bindsocket(int port, int backlog){
	int r;
	int nfd;
	nfd = socket(AF_INET, SOCK_STREAM, 0);
	if (nfd < 0) {
		return -1;
	}
	
	int one=1;
	r = setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	
	r = bind(nfd, (struct sockaddr *)&addr, sizeof(addr));
	if (r < 0) return -1;
	r = listen(nfd, backlog);
	if (r < 0) return -1;
	
	int flags;
	if ((flags = fcntl(nfd, F_GETFL, 0)) < 0
		|| fcntl(nfd, F_SETFL, flags|O_NONBLOCK) < 0){
		return -1;
	}
	
	return nfd;
}
//#define MULTI_THREAD_SUPPORT 1
int httpserver_start(int port, int nthreads, int backlog) {
	int r, i;
	int nfd = httpserver_bindsocket(port, backlog);
	if (nfd < 0) return -1;

#ifdef MULTI_THREAD_SUPPORT
	pthread_t ths[nthreads];
	for (i=0; i<nthreads; i++) {
		struct event_base *base = event_init();
		if (base == NULL) return -1;
		struct evhttp *httpd = evhttp_new(base);
		if (httpd == NULL) return -1;
		r = evhttp_accept_socket(httpd, nfd);
		if (r != 0) return -1;
		 evhttp_set_gencb(httpd, httpserver_generic_handler, NULL);
		r = pthread_create(&ths[i], NULL, httpserver_dispatch, base);
		if (r != 0) return -1;
	}
	
	for (i=0; i<nthreads; i++) {
		pthread_join(ths[i], NULL);
	}
#else
	struct event_base *base = event_init();
	if (base == NULL) return -1;
	struct evhttp *httpd = evhttp_new(base);
	if (httpd == NULL) return -1;
	r = evhttp_accept_socket(httpd, nfd);
	evhttp_set_gencb(httpd, httpserver_generic_handler, NULL);
	httpserver_dispatch(base);
#endif
}

void *httpserver_dispatch(void *arg) {
#ifndef MULTI_THREAD_SUPPORT
	struct event_base *base = (struct event_base *)arg;
	struct event *signal_event;
	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return NULL;
	}
#endif
	event_base_dispatch((struct event_base *) arg);

#ifndef MULTI_THREAD_SUPPORT
	//evconnlistener_free(lmg_listener);
	event_free(signal_event);
#endif
	event_base_free((struct event_base *) arg);
	return NULL;
}

void httpserver_generic_handler(struct evhttp_request *req, void *arg) {
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	httpserver_process_request(req);
}

void httpserver_process_request(struct evhttp_request *req) {
	
	//Process http request
	//...
	pthread_t tid = pthread_self();
	printf("tid=%ld\n", tid);
	
	//char *post = EVBUFFER_DATA(req->input_buffer);
	//Get evbuffer data as post client data
	size_t length = evbuffer_get_length(req->input_buffer);
	if (length <= 0) {
		printf("recv http response len=%lu completely\n", length);
		return;
	}
	//获取post json数据
	char *buffer = malloc(length+1);
	if (NULL == buffer) {
		return;
	}
	evbuffer_copyout(req->input_buffer, buffer, length);
	buffer[length] = 0;
	printf("recv http response [%s] len=%lu completely\n", buffer, length);
	
	//解析post json数据
	dhcp_pair *pair = malloc(sizeof(dhcp_pair));
	if (NULL == pair) {
		return;
	}
	memset(pair, 0, sizeof(dhcp_pair));
	parse_request_json(buffer, pair);
	
	free(buffer);
	
	dhcp_leases_get(pair);
	if (pair->length == 0) {
		return;
	}
	
	struct evbuffer *buf = evbuffer_new();
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	if (buf == NULL) return;
	char *response = form_response_json(pair);
	//printf("post=[%s]\n", post);
	
	printf("response=[%s]\n", response);
	//evbuffer_add_printf(buf, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));
	evbuffer_add_reference(buf, response, strlen(response), _buffer_clean, NULL);
	//free(response);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
	//evbuffer_add_reference(req->buffer_out, response, strlen(response), _buffer_clean, NULL);
	//evhtp_send_reply(req, EVHTP_RES_OK);
	free(pair);
}

//*********************** 周期性读取/tmp/dhcp.lease **********************

void update_dhcp_table() {
	
}
//**********************************************************************

#define PORT 3901
#define BACKLOG 1024
#define THREAD_NUM 2

int main(void) {
	
	httpserver_start(PORT, THREAD_NUM, BACKLOG);
	printf("End\n");
}
