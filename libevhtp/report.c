#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <evhtp.h>

static evbase_t           * rt_evbase = NULL;
static evhtp_connection_t * rt_conn = NULL;
static evhtp_request_t    * rt_request = NULL;
char *rt_host = NULL;

static struct event *rt_evtimer = NULL;
#define RT_TIMEOUT  10

static void
lmrt_sigint(int sig, short why, void * data) {
	fprintf(stdout, "catch SIGINT signal, exit lmguard process!!\n");
	event_base_loopexit(data, NULL);
}

static void
lmrt_request_cb(evhtp_request_t * req, void * arg) {
	unsigned int length = evbuffer_get_length(req->buffer_in);
	char *buffer = malloc(length+1);
	evbuffer_copyout(req->buffer_in, buffer, length);
	buffer[length] = 0;
	printf("recv http response [%s] len=%u completely\n", buffer, length);
	free(buffer);
}

static void _buffer_clean(const void *data, size_t datalen, void *extra)
{
	printf("buffer_clean=data(%s), datalen(%d), extra(%s)\n", data, datalen, extra);
	free((void *)data);
}

#include <inttypes.h>
static evhtp_res
print_data(evhtp_request_t * req, evbuf_t * buf, void * arg) {
    printf("Got %zu bytes\n", evbuffer_get_length(buf));

    return EVHTP_RES_OK;
}

static evhtp_res
print_new_chunk_len(evhtp_request_t * req, uint64_t len, void * arg) {
    printf("started new chunk, %" PRIu64 "  bytes\n", len);

    return EVHTP_RES_OK;
}

static evhtp_res
print_chunk_complete(evhtp_request_t * req, void * arg) {
    printf("ended a single chunk\n");

    return EVHTP_RES_OK;
}


static evhtp_res
print_chunks_complete(evhtp_request_t * req, void * arg) {
    printf("all chunks read\n");
	unsigned int length = evbuffer_get_length(req->buffer_in);
	char *buffer = malloc(length+1);
	evbuffer_copyout(req->buffer_in, buffer, length);
	buffer[length] = 0;
	printf("============== [%s] len=%u completely\n", buffer, length);
	free(buffer);
    return EVHTP_RES_OK;
}

static void lmrt_timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	printf("timeout_callback\n");
	evbase_t *evbase = (evbase_t *)arg;

	//New a http connection to server
	rt_conn = evhtp_connection_new(evbase, rt_host, 80);
	if (!rt_conn){
		printf("call evhtp_connection_new error, %p", rt_conn);
		return;
	}

	//Set timeout for http client read and write
	struct timeval tv_w, tv_r;
	tv_w.tv_sec = 6; 
	tv_r.tv_sec = 10;
	tv_w.tv_usec = 0;
	tv_r.tv_usec = 0;
	evhtp_connection_set_timeouts(rt_conn, &tv_r, &tv_w);

	//New a http request data
	rt_request = evhtp_request_new(lmrt_request_cb, evbase);
	if (!rt_request){
		printf("call evhtp_request_new error,%p", rt_request);
		goto EXIT1;
	}
	//Register hooks to process response from server.
	// All of below hooks not work.
	//evhtp_set_hook(&rt_request->hooks, evhtp_hook_on_read, print_data, evbase);
    //evhtp_set_hook(&rt_request->hooks, evhtp_hook_on_new_chunk, print_new_chunk_len, NULL);
    //evhtp_set_hook(&rt_request->hooks, evhtp_hook_on_chunk_complete, print_chunk_complete, NULL);
	//evhtp_set_hook(&rt_request->hooks, evhtp_hook_on_chunks_complete, print_chunks_complete, NULL);
	
	evhtp_headers_add_header(rt_request->headers_out,
		evhtp_header_new("Host", "magicwifi.com.cn", 0, 0));
	evhtp_headers_add_header(rt_request->headers_out,
		evhtp_header_new("Content-Type", "text/html", 0, 0));
	evhtp_headers_add_header(rt_request->headers_out,
		evhtp_header_new("User-Agent", "libevhtp", 0, 0));
	evhtp_headers_add_header(rt_request->headers_out,
		evhtp_header_new("Connection", "close", 0, 0));

	//Set Content-Length for json body length
	size_t content_length = strlen("Hello World");
	char cl_buf[8];
	snprintf(cl_buf, sizeof(cl_buf), "%u", content_length);
	evhtp_headers_add_header(rt_request->headers_out,
		evhtp_header_new("Content-Length", cl_buf, 0, 0));

	//Bind @rt_conn and @rt_request;
	//Send http post request head to server;
	evhtp_make_request(rt_conn, rt_request, htp_method_POST, "/reportterminal.cgi");

	//Send http post request body
	char *data = NULL;
	int length = 100;
	data = malloc(length);
	memset(data, 0, length);
	memcpy(data, "Hell WOrld", strlen("Hell WOrld"));
	evbuffer_add_reference(bufferevent_get_output(rt_conn->bev), data,
		content_length, _buffer_clean, NULL);
	rt_request = NULL; // it had been pointed to @rt_conn->request
	
	// If the http client closed, evhtp_connection_free() is called by _evhtp_connection_eventcb() callback func
	rt_conn=NULL;
	return;
EXIT1:
	evhtp_connection_free(rt_conn);
	rt_conn = NULL;
}

struct in_addr * wd_gethostbyname1(const char *name)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct in_addr *h_addr;
	int  s, i=0;
	
	/* XXX Calling function is reponsible for free() */

	h_addr = malloc(sizeof(struct in_addr));
		
	while (1) {
		i++;
			
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
		hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
		hints.ai_flags = 0;
		hints.ai_protocol = 0;          /* Any protocol */
		
		s = getaddrinfo(name, NULL, &hints, &result);
	
		if (s != 0) {
			sleep(8);
			//ERR_BUG1("wd_gethostbyname1 failed try %d", i);
			continue;
		} 

		//ERR_BUG1("wd_gethostbyname1 sucess break %d", i);
		break;  /*exit crycle*/
		
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
		//const char *ip = inet_ntoa(addr->sin_addr);
		//fprintf(stdout, "ip=%s, name=%p\n", ip, rp->ai_canonname);
		*h_addr = addr->sin_addr;
		break;
	}
	
	freeaddrinfo(result);		
	return h_addr;
}

int main(int argc, char **argv){
	fprintf(stdout, "lmg_terminal startup\n");

	//signal capture
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
		fprintf(stderr, "signal SIGPIPE %s\n", strerror(errno));;
		return -1;
	}
	
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	
	// get host ip by dns
	struct in_addr *h_addr;
	h_addr = wd_gethostbyname1("magicwifi.com.cn");
	rt_host = strdup(inet_ntoa(*h_addr));
	printf("rt_host=%s\n", rt_host);
	free(h_addr);

	rt_evbase = event_base_new();
	if (!rt_evbase){
		printf("report event_base_new() error");
		goto EXIT;
	}

	// register SIGINT signal
	struct event *ev_sigint;
	ev_sigint = evsignal_new(rt_evbase, SIGINT, lmrt_sigint, rt_evbase);
	evsignal_add(ev_sigint, NULL);

	struct timeval tv;
	rt_evtimer = event_new(rt_evbase, -1, EV_PERSIST, lmrt_timeout_cb, rt_evbase);
	if (!rt_evtimer){
		printf("report new rt_evtimer failed, exit it");
		goto EXIT;
	}

	evutil_timerclear(&tv);
	tv.tv_sec = RT_TIMEOUT;
	event_add(rt_evtimer, &tv);
	
	event_base_loop(rt_evbase, &tv);

	if (rt_evtimer) event_free(rt_evtimer);
	event_base_free(rt_evbase);

EXIT:
	if (rt_host)
		free(rt_host);
	rt_host= NULL;
	return 0;
}

