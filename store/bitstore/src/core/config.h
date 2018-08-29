#ifndef BITSTORE_CORE_H
#define BITSTORE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "constants.h"
#include "dictionary.h"

#define DEFAULT_SERVER_ADDR                 ("0.0.0.0:8080")
#define DEFAULT_AIO_MAX_QUEUE_DEPTH         1024
#define DEFAULT_AIO_TIMEOUT                 60
#define DEFAULT_AIO_REAP_MAX                16
#define DEFAULT_LOG_DIR                     ("/home/xiaoju/bitstore/log")
#define DEFAULT_LOG_SIZE                    INT_LEN_1024
#define DEFAULT_LOG_FLUSH_INTERVAL          0

typedef struct {
    char addr[INT_LEN_256];
}server_t;

typedef struct {
    int max_queue_depth;
    int timeout;
    int aio_reap_max;
}libaio_t;

typedef struct {
    char dir[INT_LEN_256];
    int  size;
    int  flush_interval;
}log_t;

typedef struct {
    server_t server;
    libaio_t aio;
    log_t    log;
}config_t;

typedef struct {
    int      argc;
    char     **argv;
    config_t config;
}bitstore_context_t;

int parse_config(config_t *config, const char *file);
void parse_server(server_t *server, dictionary *ini);
void parse_aio(libaio_t *aio, dictionary *ini);
void parse_log(log_t *log, dictionary *ini);

#ifdef __cplusplus
}
#endif

#endif  // BITSTORE_CORE_H
