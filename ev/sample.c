/*************************************************************************
	> File Name: sample.c
	> Author: 
	> Mail: 
	> Created Time: 2016年05月25日 星期三 09时58分05秒
 ************************************************************************/

#include<stdio.h>
#include <ev.h>
#include <signal.h>
#include <sys/unistd.h>

ev_io io_w;
ev_timer timer_w;
ev_signal signal_w;

void io_action(struct ev_loop *main_loop, ev_io *io_w, int e)
{
    int rst;
    char buf[1024] = {'\0'};
    puts("in io cb\n");

    read(STDIN_FILENO, buf, sizeof(buf));
    buf[1023] = '\0';
    printf("Read in a string %s \n", buf);

    ev_io_stop(main_loop,io_w);
}

void timer_action(struct ev_loop *main_loop, ev_timer *timer_w, int e)
{
    puts("in timer cb\n");
    ev_timer_stop(main_loop, timer_w);
}

void signal_action(struct ev_loop *main_loop, ev_signal signal_w, int e)
{
    puts("in signal cb\n");
    ev_signal_stop(main_loop, &signal_w);
    ev_break(main_loop, EVBREAK_ALL);
}

int main(int argc, char *argv[])
{
    struct ev_loop *main_loop = ev_default_loop(0);

    ev_init(&io_w, io_action);
    ev_io_set(&io_w, STDIN_FILENO, EV_READ);

    ev_init(&timer_w, timer_action);
    ev_timer_set(&timer_w, 2, 0);
    
    ev_init(&signal_w, signal_action);
    ev_signal_set(&signal_w, SIGINT);

    ev_io_start(main_loop, &io_w);
    ev_timer_start(main_loop, &timer_w);
    ev_signal_start(main_loop, &signal_w);

    ev_run(main_loop, 0);

    return 0;
    
}
