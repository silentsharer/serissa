#include "log.h"
#include <stdarg.h>
#include "core/constants.h"
#include <glog/logging.h>

void logf(log_level_t level, const char *fmt, ...)
{
    char buf[INT_LEN_256] = {0};
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    switch (level)
    {
        case LOG_INFO: LOG(INFO) << buf; break;
        case LOG_WARNING: LOG(WARNING) << buf; break;
        case LOG_ERROR: LOG(ERROR) << buf; break;
        case LOG_FATAL: LOG(FATAL) << buf; break;
        default:LOG(INFO) << buf;
    }
}
