#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <core/config.h>
#include <errno.h>

#include "blockdevice.h"
#include "error/error.h"

int block_device_aio_start(block_device_t *block_device)
{
    int ret = 0;

    block_device->aio_stop = false;

    ret = aio_queue_init(&block_device->aio_queue, block_device->bctx->config.aio.max_queue_depth);
    if (ret != BITSTORE_OK) {
        return ret;
    }

    ret = thread_create(&block_device->thread_id, (void*)block_device_aio_thread, block_device);
    if (ret != BITSTORE_OK) {
        aio_queue_destroy(&block_device->aio_queue);
        return ret;
    }

    return BITSTORE_OK;
}

void block_device_aio_stop(block_device_t *block_device)
{
    block_device->aio_stop = true;
    aio_queue_destroy(&block_device->aio_queue);
}

void block_device_aio_thread(void *arg)
{
    int ret = 0;
    block_device_t *block_device = (block_device_t*) arg;
    int timeout = block_device->bctx->config.aio.timeout;
    int max = block_device->bctx->config.aio.aio_reap_max;

    aio_t **paio = (aio_t**)malloc(sizeof(aio_t*) * max);
    if (paio == NULL) {
        return;
    }

    while (!block_device->aio) {
        ret = aio_queue_getevents(&block_device->aio_queue, paio, timeout, max);
        if (ret < 0) {
            continue;
        }

        for (int i = 0; i < ret; i++) {
            aio_context_t *aioctx = (aio_context_t*)paio[i]->priv;
            long r = aio_return_value(paio[i]);
            if (r < 0) {
                aioctx->rval = -EIO;
            } else if (paio[i]->length != r) {
            }

            aio_context_try_wake(aioctx);
        }
    }

    free(paio);
    paio = NULL;
}

int block_device_open(block_device_t *block_device, const char *path)
{
#ifdef HAVE_LIBAIO
    block_device->aio = true;
    block_device_aio_open(block_device, path);
#endif
    if (!block_device->aio) {
        return BITSTORE_ERR_BLOCK_INIT_NOIO;
    }
    return BITSTORE_OK;
}

int block_device_aio_open(block_device_t *block_device, const char *path)
{
    int ret = 0;

    block_device->fd_direct = open(path, O_RDWR|O_DIRECT);
    if (block_device->fd_direct < 0) {
        return BITSTORE_ERR_FILE_OPEN;
    }

    struct stat st;
    ret = fstat(block_device->fd_direct, &st);
    if (ret < 0) {
        return BITSTORE_ERR_FILE_STAT;
    }

    ret = block_device_aio_start(block_device);
    if (ret != BITSTORE_OK) {
        return ret;
    }

    return BITSTORE_OK;
}

// data must be free by caller
int block_device_aio_read(block_device_t *block_device, aio_context_t *aioctx,
                          uint64_t offset, uint64_t length, void **data)
{
    int ret = 0;

    ret = aio_context_add(aioctx, block_device->fd_direct, data);
    if (ret != BITSTORE_OK) {
        return ret;
    }

    aio_pread(&aioctx->aios[aioctx->num_pending], offset, length);
    *data = aioctx->aios[aioctx->num_pending].buf;

    return BITSTORE_OK;
}

int block_device_aio_write(block_device_t *block_device, aio_context_t *aioctx,
                           uint64_t offset, uint64_t length, void *data)
{
    int ret = 0;

    ret = aio_context_add(aioctx, block_device->fd_direct, data);
    if (ret != BITSTORE_OK) {
        return ret;
    }

    aio_pwrite(&aioctx->aios[aioctx->num_pending], offset, length);

    return BITSTORE_OK;
}

int block_device_aio_submit(block_device_t *block_device, aio_context_t *aioctx)
{
    int ret = 0;

    if (aioctx->num_pending == 0) {
        return 0;
    }

    struct iocb *iocbs[1000] = {0};
    for (int i = 0; i < aioctx->num_pending; i++) {
        iocbs[i] = &aioctx->aios[i].iocb;
    }

    ret = aio_queue_submit(&block_device->aio_queue, iocbs, aioctx->num_pending);
    if (ret < 0) {
        return ret;
    }

    if (ret < aioctx->num_pending) {
        return BITSTORE_ERR_AIO_EIO;
    }

    return BITSTORE_OK;
}