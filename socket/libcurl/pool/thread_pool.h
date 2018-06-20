#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <iostream>
#include <list>

using namespace std;


class CJob{
    public:
        CJob(void* (*r)(void* arg), void* a): callback_routine(r), arg(a){}
        ~CJob(){}

        void* (*callback_routine) (void* arg);
        void* arg;
};

//fixed size thread pool
class CThreadPool{
    public:
        CThreadPool(int max_th_num);
        ~CThreadPool();

        int pool_add_job(void* (*process)(void* arg), void* arg);

        pthread_mutex_t      queue_mutex;
        pthread_cond_t       queue_cond;
        list<CJob*>          queue_job;
        pthread_t*           thread_vec;
        int                  max_thread_num;
        int                  cur_queue_size;
        int                  shutdown;
};

#endif
