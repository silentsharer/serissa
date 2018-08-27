#ifndef BITSTORE_CORE_H
#define BITSTORE_CORE_H

#include "constants.h"
#include "dictionary.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_SERVER_ADDR                 ("0.0.0.0:8080")
#define DEFAULT_LOG_DIR                     ("/home/xiaoju/bitstore/log")
#define DEFAULT_LOG_SIZE                    INT_LEN_1024
#define DEFAULT_LOG_FLUSH_INTERVAL          0

typedef struct {
    char addr[INT_LEN_256];
}server_t;

typedef struct {
    char dir[INT_LEN_256];
    int  size;
    int  flush_interval;
}log_t;

typedef struct {
    server_t    server;
    log_t       log;
}config_t;

int parse_config(config_t *config, const char *file);
void parse_server(server_t *server, dictionary *ini);
void parse_log(log_t *log, dictionary *ini);

#ifdef __cplusplus
}
#endif

#endif  // BITSTORE_CORE_H
