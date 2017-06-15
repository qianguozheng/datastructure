#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

#include <evhtp.h>

#define WORK_PROCESSES_NUM 2
static char log_path[32];
static struct event_base *lmg_base;
static struct evconnlistener *lmg_listener;



//////////////// log.c
#include <sys/time.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include <event2/util.h>

static FILE *lmfp = NULL;
static off_t log_max_size = 1024*1024;

int log_level = 0;  // 0 : DEBUG, 1:MSG, 2:WARN, 3:ERR 

static char log_desc[5][8] = {
	"debug", "msg", "warn", "err", "over"
};

#include <sys/time.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include <event2/util.h>

size_t __time_to_str(char *out, size_t osize, time_t sec, const char *fmt)
{
	struct tm tf;
	localtime_r(&sec, &tf);
	return strftime(out, osize, fmt, &tf);
}

static inline void cleanup_logfile(void)
{
	struct stat st;
	int mret;
	
	mret = fstat(fileno(lmfp), &st);
	if (mret != 0) return;
	if (st.st_size < log_max_size)  return;

	// truncate /tmp/xxxx.log
	errno = 0;
	rewind(lmfp);
	mret = errno;
	if (errno != 0 || ftruncate(fileno(lmfp), 0) != 0) ;
	fprintf(lmfp, "clear log  %d(%s), %d (%s)\n", mret, strerror(mret), errno, strerror(errno));
	fflush(lmfp);
}

// log callback for libevent ;
// @severity = 0 --3  (0 : DEBUG, 1:MSG, 2:WARN, 3:ERR )
void write_to_file_cb(int severity, const char *msg)
{
	if (severity < log_level) {
		return;
	}

	if (!lmfp) return;
	cleanup_logfile();

	struct timeval cur_tv;
	char tv[32], dt[42];
	gettimeofday(&cur_tv, NULL);
	__time_to_str(tv, sizeof(tv), cur_tv.tv_sec, "%Y-%m-%d %H:%M:%S");
	snprintf(dt, sizeof(dt), "%s.%06u", tv, (unsigned int)(cur_tv.tv_usec));

	fprintf(lmfp, "%s|%s|%s\n", dt, log_desc[severity], msg); 
	fflush(lmfp);
}

// @logname = "/tmp/lmguard00x.log"
int lmg_init_log(const char *logname, off_t max_size)
{
	lmfp = fopen(logname, "a+");
	if (!lmfp) {
		fprintf(stderr, "fopen %s error %s\n", logname, strerror(errno));
		return -1;
	}

	evutil_make_socket_closeonexec(fileno(lmfp));

	log_max_size = max_size;
	return 0;
}

void lmg_release_log(void)
{
	if (!lmfp)  return;
	
	fclose(lmfp);
	lmfp = NULL;
}


////////////////
static evhtp_t * yshtp    = NULL;

static void _buffer_clean(const void *data, size_t datalen, void *extra)
{
	printf("buffer_clean=data(%s), datalen(%d), extra(%s)\n", data, datalen, extra);
	free((void *)data);
}
#if 0
static void
lmg_default_cb(evhtp_request_t * req, void * arg) 
{
	struct sockaddr_in *inaddr =(struct sockaddr_in *)req->conn->saddr;
	const char *docroot = arg;
	int nlen = 0;

	char *buffer = malloc(512);
	nlen = snprintf(buffer, 512, "Welcome %s:%u! access [%s/%s].", inet_ntoa(inaddr->sin_addr), 
		ntohs(inaddr->sin_port), docroot, req->uri->path->full);
	_event_debugx("path:%s, file:%s\n", req->uri->path->path, req->uri->path->file);
	_event_debugx("%s    %d\n", buffer, nlen);
	evbuffer_add_reference(req->buffer_out,
		buffer, nlen, _buffer_clean, NULL);

	evhtp_send_reply(req, EVHTP_RES_OK);
}
#endif
#define HTTP_VL_LEN 48
static inline void ys_make_html_head(evbuf_t *outbuf, const char *title)
{
	evbuffer_add_printf(outbuf, "<html>\n<head>\n<title>%s</title></head>\n", title);
}

static void ys_make_comm_body(evhtp_request_t *req, const char *title, const char *msg)
{
	evhtp_headers_add_header(req->headers_out,
		evhtp_header_new("Server","Lemon MagicWiFi", 0, 0));
	evhtp_headers_add_header(req->headers_out, 
		evhtp_header_new("Content-Type","text/html", 0, 0));
	ys_make_html_head(req->buffer_out, title);
	evbuffer_add_printf(req->buffer_out, "<body>%s</body>\n</html>", msg);
}

static void lmg_http_send_html(evhtp_request_t *req, int rescode, const char *title, const char *msg)
{
	ys_make_comm_body(req, title, msg);
	evhtp_send_reply(req, rescode);
}

static void lmg_http_send_redirect(evhtp_request_t *req, int rescode,
	const char *url, const char *title)
{
	evhtp_headers_add_header(req->headers_out,
		evhtp_header_new("Server", "MagicWiFi", 0, 0));
	evhtp_headers_add_header(req->headers_out,
		evhtp_header_new("Location",url, 0, 1));
	evhtp_headers_add_header(req->headers_out,
		evhtp_header_new("Content-Type", "text/html", 0, 0));

	evbuffer_add_printf(req->buffer_out, "<html>\r\n<head>\r\n<title>%s</title>\r\n", title);
	evbuffer_add_printf(req->buffer_out, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\r\n</head>");

	evbuffer_add_printf(req->buffer_out, "<body>\r\n<h2>\r\n");
	evbuffer_add_printf(req->buffer_out, "Please <a href='%s'>click here</a>.", url);
	evbuffer_add_printf(req->buffer_out, "</h2>\r\n</body>\r\n</html>\r\n");

	evhtp_send_reply(req, rescode);
}

static void 
lmg_default_portal(evhtp_request_t *req, void *arg)
{
	lmg_http_send_html(req, EVHTP_RES_OK, "Richard", "Hello World");
}

int init_http_server(struct event_base *evbase, const char *bindip, unsigned short port)
{
	//INIT_LIST_HEAD(&g_msg_list); 

	yshtp    = evhtp_new(evbase, NULL);
	if (yshtp == NULL) {
		return -1;
	}

	evhtp_set_max_keepalive_requests(yshtp, 1);  // disable keepalive  for HTTP "Connection: close"
	evhtp_set_max_body_size(yshtp, 4096);

	// set timeout for http client read and write
	struct timeval tv_w, tv_r;
	tv_w.tv_sec = 6; tv_r.tv_sec = 10;
	tv_w.tv_usec = 0;
	tv_r.tv_usec = 0;
	evhtp_set_timeouts(yshtp, &tv_r, &tv_w);

	/* set a default request handler */
	//evhtp_set_gencb(yshtp, ys_default_cb, "/tmp");
	//evhtp_set_gencb(yshtp, lmg_default_cb001, "/tmp");
	evhtp_set_gencb(yshtp, lmg_default_portal, "/tmp");

	 if (evhtp_bind_socket(yshtp, bindip, port, 128) < 0) {
	 	evhtp_free(yshtp);
		event_warnx("Could not bind http socket %s\n", strerror(errno));
		return -1;
	 }

	 evutil_socket_t lfd = evconnlistener_get_fd(yshtp->server);
	 _event_debugx("listen %u fd=%d\n", port, lfd);
	
	return 0;
}

void release_http_server(void)
{
	// clear g_msg_list  list
	
	if (!yshtp)  return;
	
	evhtp_unbind_socket(yshtp);
	evhtp_free(yshtp);
}

static void
lmg_sigint(int sig, short why, void * data) {
	fprintf(stdout, "catch SIGINT signal, exit lmguard process!!\n");
	event_base_loopexit(data, NULL);
}


//Event to do schedule things
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>

struct event *lmg_heartbeat;

void heartbeat_timeout_cb(evutil_socket_t fd, short event, void *params)
{
	fprintf(stdout, "pid=%d\n", getpid());
	//fprintf(stdout, "fd=%d, event=%d, params=%s\n", fd, event, params);
}
void heartbeat_init(struct event_base *base)
{
	fprintf(stdout, "lmg_hearbeat start\n");

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	lmg_heartbeat = event_new(base, -1, 0, NULL, base);
	
	event_assign(lmg_heartbeat, base, -1, EV_PERSIST, heartbeat_timeout_cb, (void*)base);
	evutil_timerclear(&tv);
	tv.tv_sec = 2; //Must after evutil_timerclear.
	event_add(lmg_heartbeat, &tv);
}



int main(int argc, char **argv)
{
	int ret = 0;
	fprintf(stdout, "hydra startup\n");

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
		fprintf(stderr, "signal SIGPIPE %s\n", strerror(errno));
		return -1;
	}

	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	//daemon(1, 1);
	if (lmg_init_log("test.log", (5*1024*1024)) != 0){
		fprintf(stderr, "log init failed\n");
		return -1;
	}
	event_set_log_callback(write_to_file_cb);
	

	// init listen  server for lmg author
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(8089);

	lmg_base = event_base_new();
	if (!lmg_base){
		event_warn("lmgauth event_base_new() error\n");
		goto _EXIT_1;
	}
	
	//register SIGINT signal
	struct event *ev_sigint;
	ev_sigint = evsignal_new(lmg_base, SIGINT, lmg_sigint, lmg_base);
	evsignal_add(ev_sigint, NULL);
#if 0
	// register heartbeat connection
	lmg_listener = evconnlistener_new_bind(lmg_base, lmg_acceptcb, lmg_base, 
		LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_EXEC,
		-1, (struct sockaddr *)&sin, sizeof(sin));

	if (!lma_listener) {
		event_warn("lmg evconnlistener_new_bind() error\n");
		goto _EXIT_2;
	}
#endif
	heartbeat_init(lmg_base);
	if (init_http_server(lmg_base, "127.0.0.1", 8088) != 0){
		goto _EXIT_3;
	}

		// fork some child worker processes
	int i;
	pid_t pid;
	for (i = 0; i < (WORK_PROCESSES_NUM-1); i ++) {
		pid = fork();
		switch (pid) {
		case -1:
			event_warn("fork() failed while spawning\n");
			goto _EXIT_3;
			break;

		case 0:
			_event_debugx("the child process run this pid=%u\n", pid);
			if (event_reinit(lmg_base) < 0) {
				event_warn("call event_reinit faild after fork\n");
				exit(1);
			}
			goto _CHILD_CONT;
			break;

		default:
			_event_debugx("fork a child process pid=%u\n", pid);
			break;
		}
	}

	
_CHILD_CONT:
	// reopen log file 
	lmg_release_log();
	pid = getpid();
	snprintf(log_path, sizeof(log_path), "/tmp/lmguard%u.log", pid);
	if (lmg_init_log(log_path, (5*1024*1024)) != 0) {
		goto _EXIT_3;
	}
	
#if 0
	// set LEV_OPT_CLOSE_ON_EXEC , after fork child 
	evutil_socket_t lfd = evconnlistener_get_fd(lmg_listener);
	if (evutil_make_socket_closeonexec(lfd) < 0) {
		event_warn("set lmg_listener FD_CLOEXEC error\n");
		goto _EXIT_3;
	}	
	//_event_debugx("lmg_listener->errorcb=%p\n", lmg_listener->errorcb);
	//evconnlistener_set_error_cb(lmg_listener, NULL);
	_event_debugx("listen %d fd=%d\n", 8088, lfd);
	
#endif
	event_base_dispatch(lmg_base);
	release_http_server();


_EXIT_3:
	//evconnlistener_free(lmg_listener);	
_EXIT_2:
	event_free(ev_sigint);
	event_base_free(lmg_base);
_EXIT_1:

	lmg_release_log();
}
