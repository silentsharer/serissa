#ifndef BITSTORE_LOG_H
#define BITSTORE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
}log_level_t;

void logf(log_level_t level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // BITSTORE_LOG_H
