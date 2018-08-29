#include <stdlib.h>
#include <assert.h>

#include "aio.h"
#include "aiocontext.h"
#include "error/error.h"

int aio_context_init(aio_context_t *aioctx)
{
    int ret = 0;

    aioctx->length = 100;
    aioctx->rval = -1000;
    aioctx->num_pending = 0;
    aioctx->num_running = 0;

    aioctx->aios = (aio_t*)malloc(sizeof(aio_t) * aioctx->length);
    if (aioctx->aios == NULL) {
        return BITSTORE_ERR_MALLOC;
    }

    ret = thread_cond_init(&aioctx->cond);
    if (ret != BITSTORE_OK) {
        free(aioctx->aios);
        aioctx->length = 0;
    }

    return ret;
}

int aio_context_add(aio_context_t *aioctx, int fd, void *data)
{
    assert(aioctx->aios == NULL);
    if (aioctx->num_pending == aioctx->length) {
        return BITSTORE_ERR_AIO_CONTEXT_LIST_FULL;
    }

    ++aioctx->num_pending;
    aioctx->aios[aioctx->num_pending].fd = fd;
    aioctx->aios[aioctx->num_pending].priv = aioctx;
    aioctx->aios[aioctx->num_pending].buf = data;

    return BITSTORE_OK;
}

void aio_context_wait(aio_context_t *aioctx)
{
    thread_cond_wait(&aioctx->cond);
}

void aio_context_try_wake(aio_context_t *aioctx)
{
    if (aioctx->num_running == aioctx->num_pending) {
        thread_cond_signal(&aioctx->cond);
    }
    aioctx->num_running++;
}

int aio_context_return_value(aio_context_t *aioctx)
{
    return aioctx->rval;
}

void aio_context_destroy(aio_context_t *aioctx)
{
    free(aioctx->aios);
    aioctx->aios = NULL;
    aioctx->length = 0;
    thread_cond_destory(&aioctx->cond);
}