#include "thread_pool.h"

static void* thread_routine(void* arg){
    CThreadPool* pool = (CThreadPool*)arg;
    if(pool == NULL) return NULL;
    printf("starting thread [%lu]\n", pthread_self());

    while(1){
        pthread_mutex_lock(&pool->queue_mutex);
        while (pool->cur_queue_size == 0 && !pool->shutdown){
            printf("thread [%lu] is waiting\n", pthread_self());
            pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
        }

        if(pool->shutdown){
            printf("thread [%lu] will exit\n", pthread_self());
            pthread_mutex_unlock(&pool->queue_mutex);
            pthread_exit(NULL);
        }
        printf("thread [%lu] is starting to work\n", pthread_self());

        assert (pool->cur_queue_size != 0);
        assert (!pool->queue_job.empty());

        pool->cur_queue_size--;
        CJob* job = pool->queue_job.front();
        pool->queue_job.pop_front();
        pthread_mutex_unlock(&pool->queue_mutex);

        (*job->callback_routine) (job->arg);
        delete job;
    }
}


CThreadPool::CThreadPool(int max_th_num): cur_queue_size(0), shutdown(0), max_thread_num(max_th_num){
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
    thread_vec = new pthread_t[max_thread_num];
    for(int i = 0; i < max_thread_num; i++){
        pthread_create(&thread_vec[i], NULL, thread_routine, (void*)this);
    }
}

CThreadPool::~CThreadPool(){
    if(shutdown) return;
    shutdown = 1;
    pthread_cond_broadcast(&queue_cond);
    for(int i=0; i < max_thread_num; i++)
        pthread_join(thread_vec[i], NULL);
    delete [] thread_vec;
    for(list<CJob*>::iterator it = queue_job.begin(); it != queue_job.end(); ++it)
        delete *it;
    queue_job.clear();
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
}

int CThreadPool::pool_add_job(void* (*process)(void* arg), void* arg){
    CJob* job = new CJob(process, arg);

    pthread_mutex_lock(&queue_mutex);
    queue_job.push_back(job);
    cur_queue_size++;
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond);
    return 0;
}
