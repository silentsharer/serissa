#include <stdlib.h>
#include "thread.h"
#include "error/error.h"

int thread_mutex_init(thread_mutex_t *mutex)
{
    if(pthread_mutex_init(mutex, NULL) != 0) {
        return BITSTORE_ERR_THREAD_MUTEX_INIT;
    }
    return BITSTORE_OK;
}

int thread_mutex_destroy(thread_mutex_t *mutex)
{
    if (pthread_mutex_destroy(mutex) != 0) {
        return BITSTORE_ERR_THREAD_MUTEX_DESTROY;
    }
    return BITSTORE_OK;
}

int thread_mutex_lock(thread_mutex_t *mutex)
{
    if (pthread_mutex_lock(mutex) != 0) {
        return BITSTORE_ERR_THREAD_MUTEX_LOCK;
    }
    return BITSTORE_OK;
}

int thread_mutex_unlock(thread_mutex_t *mutex)
{
    if (pthread_mutex_unlock(mutex) != 0) {
        return BITSTORE_ERR_THREAD_MUTEX_UNLOCK;
    }
    return BITSTORE_OK;
}

int thread_cond_init(thread_cond_t *cond)
{
    int ret = 0;

    if ((ret = thread_mutex_init(&cond->mutex)) != BITSTORE_OK) {
        return ret;
    }

    if (pthread_cond_init(&cond->cond, NULL) != 0) {
        thread_mutex_destroy(&cond->mutex);
        return BITSTORE_ERR_THREAD_COND_INIT;
    }

    return BITSTORE_OK;
}

int thread_cond_destory(thread_cond_t *cond)
{
    int mutex_err, cond_err = 0;

    mutex_err = thread_mutex_destroy(&cond->mutex);
    if (pthread_cond_destroy(&cond->cond) < 0) {
        cond_err = BITSTORE_ERR_THREAD_COND_DESTROY;
    }

    return cond_err != BITSTORE_OK ?cond_err :mutex_err;
}

int thread_cond_wait(thread_cond_t *cond)
{
    int ret = 0;

    if ((ret = thread_mutex_lock(&cond->mutex)) != BITSTORE_OK) {
        return ret;
    }

    if (pthread_cond_wait(&cond->cond, &cond->mutex) < 0) {
        thread_mutex_unlock(&cond->mutex);
        return BITSTORE_ERR_THREAD_COND_WAIT;
    }

    if ((ret = thread_mutex_unlock(&cond->mutex)) != BITSTORE_OK) {
        return ret;
    }

    return ret;
}

int thread_cond_signal(thread_cond_t *cond)
{
    if (pthread_cond_signal(&cond->cond) < 0) {
        return BITSTORE_ERR_THREAD_COND_SIGNAL;
    }
    return BITSTORE_OK;
}

int thread_cond_broadcast(thread_cond_t *cond)
{
    if (pthread_cond_broadcast(&cond->cond) < 0) {
        return BITSTORE_ERR_THREAD_COND_BROADCAST;
    }
    return BITSTORE_OK;
}

int thread_create(pthread_t *tid_alp, void *start_function_avp, void *func_arg_avp)
{
    int ret = 0;

    pthread_attr_t pthread_attr, *attr = NULL;
    attr = &pthread_attr;

    if (pthread_attr_init(attr) < 0) {
        return BITSTORE_ERR_THREAD_INIT;
    }

    if (pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED) < 0) {
        pthread_attr_destroy(&pthread_attr);
        return BITSTORE_ERR_THREAD_INIT;
    }

    if (pthread_create(tid_alp, attr, start_function_avp, func_arg_avp) < 0) {
        ret = BITSTORE_ERR_THREAD_CREATE;
    }

    pthread_attr_destroy(&pthread_attr);

    return ret;
}

int thread_pool_init(threadpool_t *threadpool, int thread_num, void *start_function_avp, void **func_arg_avp)
{
    int ret = 0;

    if (thread_num <= 0) {
        return BITSTORE_ERR_PARAMETER;
    }

    if (start_function_avp == NULL || func_arg_avp == NULL) {
        return BITSTORE_ERR_PARAMETER;
    }

    threadpool->stop = false;
    threadpool->thread_num = thread_num;
    threadpool->start_function_avp = start_function_avp;
    threadpool->func_arg_avp = func_arg_avp;

    threadpool->threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
    if (threadpool->threads == NULL) {
        return BITSTORE_ERR_MALLOC;
    }

    for (int i = 0; i < thread_num; i++) {
        ret = thread_create(threadpool->threads+i, thread_pool_worker, threadpool);
        if (ret!= BITSTORE_OK) {
            free(threadpool->threads);
            return ret;
        }
    }

    return BITSTORE_OK;
}

void thread_pool_destroy(threadpool_t *threadpool)
{
    threadpool->stop = true;
    free(threadpool->threads);
    threadpool->threads = NULL;
}

int thread_pool_find_worker_idx(threadpool_t *threadpool, pthread_t tid)
{
    for (int i = 0; i < threadpool->thread_num; i++) {
        if (tid == threadpool->threads[i]) {
            return i;
        }
    }
    return -1;
}

void* thread_pool_worker(void *arg)
{
    threadpool_t *threadpool = (threadpool_t*)arg;
    int idx = thread_pool_find_worker_idx(threadpool, pthread_self());
    thread_func func = (thread_func)(threadpool->start_function_avp);

    while (!threadpool->stop) {
        // process task
        func(threadpool->func_arg_avp[idx]);
    }

    return threadpool;
}
