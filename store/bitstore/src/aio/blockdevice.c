#include <sys/stat.h>

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

    block_device->fd_direct = open(path, O_RDWR|O_DIRECTORY);
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

int block_device_aio_read(block_device_t *block_device)
{
}

int block_device_aio_write(block_device_t *block_device)
{
}

void block_device_aio_aubmit(block_device_t *block_device, aio_context_t *aioctx)
{
}