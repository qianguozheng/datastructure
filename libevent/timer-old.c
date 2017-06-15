#include <stdio.h>
#include <event2/event.h>

struct cb_arg{
	struct event *ev;
	struct timeval tv;
};

void timeout_cb(int fd, short event, void *params)
{
	puts("timer");
	struct cb_arg *arg = (struct cb_arg*)params;
	struct event *ev = arg->ev;
	struct timeval tv = arg->tv;
	
	evtimer_add(ev, &tv);
}

int main(){
	struct event_base *base = event_base_new();
	struct event *timeout = NULL;
	struct timeval tv = {1, 0};
	struct cb_arg arg;
	
	timeout = evtimer_new(base, timeout_cb, &arg);
	arg.ev = timeout;
	arg.tv = tv;
	
	evtimer_add(timeout, &tv);
	event_base_dispatch(base);
	evtimer_del(timeout);
	
	return 0;
}
