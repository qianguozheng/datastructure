#include <sys/types.h>
#include <event2/event-config.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>

struct timeval lasttime;
int event_is_persistent;

static void
timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	struct timeval newtime, difference;
	struct event *timeout = arg;
	double elapsed;

	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec/ 1.0e6);

	printf("timeout_cb called at %d: %.3f seconds elapsed.\n",
		(int)newtime.tv_sec, elapsed);
	lasttime = newtime;

	if (! event_is_persistent){
		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 2;
		event_add(timeout, &tv);
	}
}

int 
main(int argc, char **argv)
{
	struct event timeout;
	struct timeval tv;
	struct event_base *base;
	int flags;

	event_is_persistent = 1;
	flags = EV_PERSIST;

	base = event_base_new();
	
	event_assign(&timeout, base, -1, flags, timeout_cb, (void *)&timeout);
	
	evutil_timerclear(&tv);
	
	tv.tv_sec = 2;
	event_add(&timeout, &tv);
	evutil_gettimeofday(&lasttime, NULL);
	event_base_dispatch(base);

	return (0);
}
