#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "aio.h"

void aio_set(aio_t *aio, int fd)
{
    aio->fd = fd;
    aio->offset = 0;
    aio->length = 0;
    aio->rval = -1000;
    aio->buf = NULL;
}

void aio_memalign(void **memptr, size_t size)
{
    posix_memalign(memptr, size_t(sysconf(_SC_PAGESIZE)), size);
}

void aio_pread(aio_t *aio, uint64_t offset, uint64_t length)
{
    aio->offset = offset;
    aio->length = length;

    aio_memalign(&aio->buf, size_t(BUF_SIZE));
    io_prep_pread(&aio->iocb, aio->fd, aio->buf, size_t(aio->length), aio->offset);
}

void aio_pwrite(aio_t *aio, uint64_t offset, uint64_t length)
{
    aio->offset = offset;
    aio->length = length;

    io_prep_pread(&aio->iocb, aio->fd, aio->buf, size_t(aio->length), aio->offset);
}

long aio_return_value(aio_t *aio)
{
    return aio->rval;
}
