#include <iostream>
#include <pthread.h>
#include <event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int init_count = 0;
pthread_mutex_t init_lock;
pthread_cond_t init_cond;

using namespace std;
typedef struct{
	pthread_t thread_id;
	struct event_base *base;
	struct event notify_event;
} mythread;

void *worker_libevent(void *arg)
{
	mythread *p = (mythread *)arg;
	pthread_mutex_lock(&init_lock);
	init_count++;
	pthread_cond_signal(&init_cond);
	pthread_mutex_unlock(&init_lock);
	event_base_loop(p->base, 0);
}

int create_worker(void *(func)(void *), void *arg)
{
	mythread *p = (mythread *)arg;
	pthread_t tid;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, func, arg);
	p->thread_id = tid;
	return 0;
}

void process(int fd, short which, void *arg)
{
	mythread *p = (mythread *)arg;
	cout << "I am in the thread:" << p->thread_id << endl;
	char buffer[100];
	memset(buffer, 0, 100);
	
	int ilen = read(fd, buffer, 100);
	cout << "read num is :"<<ilen << endl;
	cout << "the buffer :" << buffer << endl;
}

int setup_thread(mythread *p, int fd)
{
	p->base = event_init();
	event_set(&p->notify_event, fd, EV_READ|EV_PERSIST, process, p);
	event_base_set(p->base, &p->notify_event);
	event_add(&p->notify_event, 0);
	
	return 0;
}

int main()
{
	struct sockaddr_in in;
	int fd;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	struct in_addr s;
	bzero(&in, sizeof(in));
	in.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void *)&s);
	in.sin_addr.s_addr = s.s_addr;
	in.sin_port = htons(20010);
	
	bind(fd, (struct sockaddr *)&in, sizeof(in));
	int threadnum = 10;
	int i;
	
	pthread_mutex_init(&init_lock, NULL);
	pthread_cond_init(&init_cond, NULL);
	mythread *g_thread;
	g_thread = (mythread *)malloc(sizeof(mythread)*10);
	for (i=0; i < threadnum; i++)
	{
		setup_thread(&g_thread[i], fd);
	}
	
	for (i=0; i < threadnum; i++)
	{
		create_worker(worker_libevent, &g_thread[i]);
	}
	
	pthread_mutex_lock(&init_lock);
	while(init_count < threadnum)
	{
		pthread_cond_wait(&init_cond, &init_lock);
	}
	pthread_mutex_unlock(&init_lock);
	
	cout <<"IN THE MAIN LOOP" << endl;
	
	string test = "I am michael";
	
	write(fd, test.c_str(), test.size());
	
	while(1)
	{
		sleep(1);
	}
	
	free(g_thread);
	return 0;
}
