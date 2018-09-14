#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include "aio.h"
#include "core/log.h"

// single asyncio operation
void aio_set(aio_t *aio, int fd)
{
    aio->fd = fd;
    aio->offset = 0;
    aio->length = 0;
    aio->rval = -1000;
    aio->priv = NULL;
    aio->buf = NULL;
}

void aio_memalign(void **memptr, size_t size)
{
    posix_memalign(memptr, (size_t)sysconf(_SC_PAGESIZE), size);
}

void aio_pread(aio_t *aio, uint64_t offset, uint64_t length)
{
    aio->offset = offset;
    aio->length = length;

    // TODO: 释放memalign分配的内存
    aio_memalign(&aio->buf, (size_t)aio->length);
    io_prep_pread(&aio->iocb, aio->fd, aio->buf, (size_t)aio->length, aio->offset);
}

void aio_pwrite(aio_t *aio, uint64_t offset, uint64_t length)
{
    aio->offset = offset;
    aio->length = length;

    io_prep_pwrite(&aio->iocb, aio->fd, aio->buf, (size_t)aio->length, aio->offset);
}

void aio_fsync(aio_t *aio)
{
    io_prep_fsync(&aio->iocb, aio->fd);
}

long aio_return_value(aio_t *aio)
{
    return aio->rval;
}

// asyncio queue operaion
int aio_queue_init(aio_queue_t *aio_queue, int max_iodepth)
{
    aio_queue->ioctx = 0;
    aio_queue->max_iodepth = max_iodepth;

    int ret = io_setup(aio_queue->max_iodepth, &aio_queue->ioctx);
    if (ret < 0) {
        if (aio_queue->ioctx) {
            io_destroy(aio_queue->ioctx);
            aio_queue->ioctx = 0;
        }
    }

    return ret;
}

int aio_queue_submit(aio_queue_t *aio_queue, struct iocb **piocb, int size)
{
    // 2^16 * 125us = ~8 seconds, so max sleep is ~16 seconds
    int attempts = 16, delay = 125;
    int left = size, done = 0;

    while (left > 0) {
        long nr = left < aio_queue->max_iodepth ?left :aio_queue->max_iodepth;
        int ret = io_submit(aio_queue->ioctx, nr, piocb + done);
        if (ret < 0) {
            if (ret == -EAGAIN && attempts-- > 0) {
                usleep(delay);
                delay *= 2;
                continue;
            }
            return ret;
        }
        done += ret;
        left -= ret;
        attempts = 16;
        delay = 125;
    }

    return done;
}

int aio_queue_getevents(aio_queue_t *aio_queue, aio_t **paio, int timeout_ms, int max)
{
    struct io_event event[max];
    struct timespec t = {
            timeout_ms / 1000,
            (timeout_ms % 1000) * 1000 * 1000
    };

    int ret = 0;
    do {
        ret = io_getevents(aio_queue->ioctx, 1, max, event, &t);
    } while(ret == -EINTR);

    for (int i = 0; i < ret; i++) {
        paio[i] = (aio_t *)event[i].obj;
        paio[i]->rval = event[i].res;
    }

    return ret;
}

void aio_queue_destroy(aio_queue_t *aio_queue)
{
    if (aio_queue->ioctx) {
        int ret = io_destroy(aio_queue->ioctx);
        assert(ret == 0);
        aio_queue->ioctx = 0;
    }
}
