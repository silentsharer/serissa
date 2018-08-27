#include "config.h"
#include "iniparser.h"
#include "error/error.h"

int parse_config(config_t *config, const char *file)
{
    dictionary *ini = iniparser_load(file);
    if (ini == NULL) {
        return BITSTORE_ERR_FILE_LOAD;
    }

    // parse section
    parse_server(&config->server, ini);
    parse_log(&config->log, ini);

    return BITSTORE_OK;
}

void parse_server(server_t *server, dictionary *ini)
{
    snprintf(server->addr, sizeof(server->addr), "%s", iniparser_getstring(ini, "server:addr", DEFAULT_SERVER_ADDR));
}

void parse_log(log_t *log, dictionary *ini)
{
    snprintf(log->dir, sizeof(log->dir), "%s", iniparser_getstring(ini, "log:dir", DEFAULT_LOG_DIR));
    log->size = iniparser_getint(ini, "log:size", DEFAULT_LOG_SIZE);
    log->flush_interval = iniparser_getint(ini, "log:flush_internal", DEFAULT_LOG_FLUSH_INTERVAL);
}