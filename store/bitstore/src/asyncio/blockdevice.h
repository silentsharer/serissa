#ifndef BITSTORE_BLOCKDEVICE_H
#define BITSTORE_BLOCKDEVICE_H

#include "core/constants.h"
#include "core/config.h"
#include "aio.h"
#include "aiocontext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool aio;
    int fd_direct;
    uint64_t size;
    bool aio_stop;
    char devname[INT_LEN_256];
    pthread_t thread_id;
    aio_queue_t aio_queue;
    bitstore_context_t *bctx;
}block_device_t;

int block_device_aio_start(block_device_t *block_device);
void block_device_aio_stop(block_device_t *block_device);
void block_device_aio_thread(void *arg);

int get_block_device_size(int fd, int64_t *psize);
int block_device_open(block_device_t *block_device, const char *path);
int block_device_aio_open(block_device_t *block_device, const char *path);
int block_device_aio_read(block_device_t *block_device, aio_context_t *aioctx,
                          uint64_t offset, uint64_t length, void **data);
int block_device_aio_write(block_device_t *block_device, aio_context_t *aioctx,
                           uint64_t offset, uint64_t length, void *data);
int block_device_aio_submit(block_device_t *block_device, aio_context_t *aioctx);
int block_device_aio_fsync(block_device_t *block_device, aio_context_t *aioctx);

#ifdef __cplusplus
}
#endif

#endif // BITSTORE_BLOCKDEVICE_H
