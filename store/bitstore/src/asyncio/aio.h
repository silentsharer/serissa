#ifndef BITSTORE_AIO_H
#define BITSTORE_AIO_H

#include <stdint.h>
#include <libaio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_SIZE  4 * 1024 * 1024

typedef struct {
    struct iocb iocb; // must be first element; see shenanigans in aio_queue_t
    void *priv;
    int fd;
    long rval;
    void *buf;
    uint64_t offset;
    uint64_t length;
}aio_t;

typedef struct {
    int max_iodepth;
    io_context_t ioctx;
}aio_queue_t;

// single asyncio operation
void aio_set(aio_t *aio, int fd);
void aio_pread(aio_t *aio, uint64_t offset, uint64_t length);
void aio_pwrite(aio_t *aio, uint64_t offset, uint64_t length);
long aio_return_value(aio_t *aio);

// asyncio queue operaion
int aio_queue_init(aio_queue_t *aio_queue, int max_iodepth);
int aio_queue_submit(aio_queue_t *aio_queue, struct iocb **piocb, int size);
int aio_queue_getevents(aio_queue_t *aio_queue, aio_t **paio, int timeout, int max);
void aio_queue_destroy(aio_queue_t *aio_queue);

#ifdef __cplusplus
}
#endif

#endif // BITSTORE_AIO_H
