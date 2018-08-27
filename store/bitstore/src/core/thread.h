#ifndef BITSTORE_THREAD_H
#define BITSTORE_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*thread_func)(void*);

typedef pthread_mutex_t thread_mutex_t;

typedef struct thread_cond_s {
    thread_mutex_t  mutex;
    pthread_cond_t  cond;
}thread_cond_t;

typedef struct {
    int         thread_num;
    pthread_t   *threads;
    void        *start_function_avp;
    void        **func_arg_avp;
    bool        stop;
}threadpool_t;

// mutex lock
int thread_mutex_init(thread_mutex_t *mutex);
int thread_mutex_destroy(thread_mutex_t *mutex);
int thread_mutex_lock(thread_mutex_t *mutex);
int thread_mutex_unlock(thread_mutex_t *mutex);

// cond
int thread_cond_init(thread_cond_t *cond);
int thread_cond_destory(thread_cond_t *cond);
int thread_cond_wait(thread_cond_t *cond);
int thread_cond_signal(thread_cond_t *cond);
int thread_cond_broadcast(thread_cond_t *cond);

// thread
int thread_create(pthread_t *tid_alp, void *start_function_avp, void *func_arg_avp);
int thread_pool_init(threadpool_t *threadpool, int thread_num, void *start_function_avp, void **func_arg_avp);
void thread_pool_destroy(threadpool_t *threadpool);
void* thread_pool_worker(void *arg);

#ifdef __cplusplus
}
#endif

#endif // BITSTORE_THREAD_H
