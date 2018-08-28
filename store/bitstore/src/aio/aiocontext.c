#include "aiocontext.h"
#include "error/error.h"

int aio_context_init(aio_context_t *aioctx)
{
    aioctx->rval = -1000;
    aioctx->num_pending = 0;
    aioctx->num_running = 0;

    return thread_cond_init(&aioctx->cond);
}

int aio_context_return_value(aio_context_t *aioctx)
{
    return aioctx->rval;
}

void aio_context_wait(aio_context_t *aioctx)
{
    while (aioctx->num_running > 0) {
        thread_cond_wait(&aioctx->cond);
    }
}

void aio_context_wake(aio_context_t *aioctx)
{
    thread_cond_signal(&aioctx->cond);
}

int aio_context_destroy(aio_context_t *aioctx)
{
    return thread_cond_destory(&aioctx->cond);
}