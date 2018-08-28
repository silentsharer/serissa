#ifndef BITSTORE_AIO_H
#define BITSTORE_AIO_H

#include <libaio.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_SIZE  4 * 1024 * 1024

typedef struct {
    int fd;
    long rval;
    void *buf;
    uint64_t offset;
    uint64_t length;
    struct iocb iocb;
}aio_t;

#ifdef __cplusplus
}
#endif

#endif // BITSTORE_AIO_H
