#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <evhtp.h>


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
int dnsquery(const char *host, int portnum, char *ip_addr)
{
   struct addrinfo hints, *result, *rp;
   char service[6];
   int retval;
   int fd;
   fd_set wfds;
   struct timeval timeout;

   int connect_failed;
   /*
    * XXX: Initializeing it here is only necessary
    *      because not all situations are properly
    *      covered yet.
    */
   int socket_error = 0;

	sprintf(service, "%d", portnum);
   memset((char *)&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_NUMERICSERV; /* avoid service look-up */
#ifdef AI_ADDRCONFIG
   hints.ai_flags |= AI_ADDRCONFIG;
#endif

   if ((retval = getaddrinfo(host, service, &hints, &result)))
   {
	  printf("Can not resolve %s: %s", host, gai_strerror(retval));
      /* XXX: Should find a better way to propagate this error. */
      errno = EINVAL;

      return(-1);
   }

   for (rp = result; rp != NULL; rp = rp->ai_next)
   {

      retval = getnameinfo(rp->ai_addr, rp->ai_addrlen,
         ip_addr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (retval)
      {
         printf("Failed to get the host name from the socket structure: %s", gai_strerror(retval));
         continue;
      }

	printf("Address=%s", ip_addr);
	if (0 == strcasecmp("::", ip_addr) ||
		0 == strcasecmp("::1", ip_addr))
	{
		printf("Address=%s, Local IP Address", ip_addr);
		continue;
	}
   }



   freeaddrinfo(result);
   
   printf("Connected to %s[%s]:%s.", host, ip_addr, service);
   return(fd);

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <inttypes.h>
#include <evhtp.h>
static void
request_cb(evhtp_request_t * req, void * arg) {
    printf("hi %zu\n", evbuffer_get_length(req->buffer_in));
}

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

    return EVHTP_RES_OK;
}

int
make_request(evbase_t         * evbase,
             evthr_t          * evthr,
             const char * const host,
             const short        port,
             const char * const path,
             evhtp_headers_t  * headers,
             evhtp_callback_cb  cb,
             void             * arg) {
    evhtp_connection_t * conn;
    evhtp_request_t    * request;

	//printf("");
    conn         = evhtp_connection_new(evbase, host, port);
    conn->thread = evthr;
    request      = evhtp_request_new(cb, arg);

    //evhtp_headers_add_header(request->headers_out,
    //                         evhtp_header_new("Host", "localhost", 0, 0));
    //evhtp_headers_add_header(request->headers_out,
    //                         evhtp_header_new("User-Agent", "libevhtp", 0, 0));
    //evhtp_headers_add_header(request->headers_out,
    //                         evhtp_header_new("Connection", "close", 0, 0));

    evhtp_headers_add_headers(request->headers_out, headers);

	evhtp_set_hook(&request->hooks, evhtp_hook_on_read, print_data, evbase);
    evhtp_set_hook(&request->hooks, evhtp_hook_on_new_chunk, print_new_chunk_len, NULL);
    evhtp_set_hook(&request->hooks, evhtp_hook_on_chunk_complete, print_chunk_complete, NULL);
    evhtp_set_hook(&request->hooks, evhtp_hook_on_chunks_complete, print_chunks_complete, NULL);
    printf("Making backend request...\n");
    
    struct timeval tv_w, tv_r;
	tv_w.tv_sec = 6; tv_r.tv_sec = 10;
	tv_w.tv_usec = 0;
	tv_r.tv_usec = 0;
	evhtp_connection_set_timeouts(conn, &tv_r, &tv_w);
	
    evhtp_make_request(conn, request, htp_method_GET, path);
    printf("backend_req sock=%d\n", conn->sock);
    printf("backend_req req sock=%d\n", request->conn->sock);
    printf("make request Ok.\n");

    return 0;
}

static void
backend_cb(evhtp_request_t * backend_req, void * arg) {
    evhtp_request_t * frontend_req = (evhtp_request_t *)arg;

	printf("frontend_req sock=%d\n", frontend_req->conn->sock);
	printf("backend_req sock=%d\n", backend_req->conn->sock);
	
	struct evbuffer *buf = evbuffer_new();
	
    evbuffer_prepend_buffer(frontend_req->buffer_out, backend_req->buffer_in);
    evhtp_headers_add_headers(frontend_req->headers_out, backend_req->headers_in);

    /*
     * char body[1024] = { '\0' };
     * ev_ssize_t len = evbuffer_copyout(frontend_req->buffer_out, body, sizeof(body));
     * printf("Backend %zu: %s\n", len, body);
     */

    
    evhtp_send_reply_body(frontend_req, frontend_req->buffer_out);
    
    evhtp_request_resume(frontend_req);
    
    printf("Backend_cb called\n");
}

static void
frontend_cb(evhtp_request_t * req, void * arg) {
    int * aux;
    int   thr;

    aux = (int *)evthr_get_aux(req->conn->thread);
    thr = *aux;

    printf("  Received frontend request on thread %d... \n", thr);

    /* Pause the frontend request while we run the backend requests. */
    evhtp_request_pause(req);

	printf("hostname=%s\n", req->uri->authority->hostname);
	printf("port=%d\n", req->uri->authority->port);
	printf("htp=%s\n", req->htp->server_name);
	printf("path=%s\n", req->uri->path->full);
	printf("frontend_cb sock=%d\n", req->conn->sock);

	if (443 == req->uri->authority->port){
		return;
	}
	evhtp_header_t * host;
	host = evhtp_headers_find_header(req->headers_in, "Host");
	printf("host=%s\n", host->val);
	
	char ip_addr[16];
	memset(ip_addr, 0, sizeof(ip_addr));
	dnsquery(host->val, 80, ip_addr);
	
    make_request(evthr_get_base(req->conn->thread),
                 req->conn->thread,
                 ip_addr, 80,
                 req->uri->path->full,
                 req->headers_in, backend_cb, req);

	//evhtp_kv_rm_and_free(req->headers_in, host);
    printf("Frontend_cb Done.\n");
}

/* Terminate gracefully on SIGTERM */
void
sigterm_cb(int fd, short event, void * arg) {
    evbase_t     * evbase = (evbase_t *)arg;
    struct timeval tv     = { .tv_usec = 100000, .tv_sec = 0 }; /* 100 ms */

    event_base_loopexit(evbase, &tv);
}

void
init_thread_cb(evhtp_t * htp, evthr_t * thr, void * arg) {
    static int aux = 0;

    printf("Spinning up a thread: %d\n", ++aux);
    evthr_set_aux(thr, &aux);
}

int
main(int argc, char ** argv) {
    struct event *ev_sigterm;
    evbase_t    * evbase  = event_base_new();
    evhtp_t     * evhtp   = evhtp_new(evbase, NULL);

	struct timeval tv_w, tv_r;
	tv_w.tv_sec = 6; tv_r.tv_sec = 10;
	tv_w.tv_usec = 0;
	tv_r.tv_usec = 0;
	evhtp_set_timeouts(evhtp, &tv_r, &tv_w);
	
    evhtp_set_gencb(evhtp, frontend_cb, NULL);

#if 0
#ifndef EVHTP_DISABLE_SSL
    evhtp_ssl_cfg_t scfg1 = { 0 };

    scfg1.pemfile  = "./server.pem";
    scfg1.privfile = "./server.pem";

    evhtp_ssl_init(evhtp, &scfg1);
#endif
#endif

    evhtp_use_threads(evhtp, init_thread_cb, 20, NULL);
#ifndef WIN32
    ev_sigterm = evsignal_new(evbase, SIGTERM, sigterm_cb, evbase);
    evsignal_add(ev_sigterm, NULL);
#endif
    evhtp_bind_socket(evhtp, "0.0.0.0", 8081, 1024);
    event_base_loop(evbase, 0);

    printf("Clean exit\n");
    return 0;
}

