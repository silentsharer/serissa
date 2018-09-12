#ifndef BITSTORE_AIOCONTEXT_H
#define BITSTORE_AIOCONTEXT_H

#include "core/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int rval;
    int length;
    int num_running;
    int num_pending;
    thread_cond_t cond;
    aio_t *aios;
}aio_context_t;

int aio_context_init(aio_context_t *aioctx);
int aio_context_return_value(aio_context_t *aioctx);
void aio_context_wait(aio_context_t *aioctx);
void aio_context_try_wake(aio_context_t *aioctx);
void aio_context_destroy(aio_context_t *aioctx);
int aio_context_add(aio_context_t *aioctx, int fd, void *data);

#ifdef __cplusplus
}
#endif

#endif // BITSTORE_AIOCONTEXT_H
