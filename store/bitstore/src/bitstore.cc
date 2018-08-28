#include <glog/logging.h>
#include <csignal>

#include <error/error.h>
#include "core/config.h"

DEFINE_string(conf, "../../conf/bitstore.conf", "program configure file");
int init(bitstore_context_t *ctx);

int main(int argc, char **argv)
{
    int ret = 0;
    bitstore_context_t ctx = {argc:argc, argv:argv};

    // init
    if ((ret = init(&ctx)) != BITSTORE_OK) {
        return ret;
    }

    return BITSTORE_OK;
}

int init(bitstore_context_t *ctx)
{
    int ret = 0;
    sigset_t signal_mask;

    // parse config
    ret = parse_config(&ctx->config, FLAGS_conf.c_str());
    if (ret != BITSTORE_OK) {
        return ret;
    }

    // init glog
    std::string log_dir(ctx->config.log.dir);
    FLAGS_max_log_size = ctx->config.log.size;
    FLAGS_logbufsecs = ctx->config.log.flush_interval;

    google::ParseCommandLineFlags(&ctx->argc, &ctx->argv, true);
    google::InitGoogleLogging(ctx->argv[0]);
    google::SetLogDestination(google::GLOG_FATAL, (log_dir + "/bitstore_fatal_").c_str());
    google::SetLogDestination(google::GLOG_ERROR, (log_dir + "/bitstore_error_").c_str());
    google::SetLogDestination(google::GLOG_WARNING, (log_dir + "/bitstore_warning_").c_str());
    google::SetLogDestination(google::GLOG_INFO, (log_dir + "/bitstore_info_").c_str());
    google::InstallFailureSignalHandler();

    // init semaphore
    signal(SIGPIPE, SIG_IGN);
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    sigaddset(&signal_mask, SIGSEGV);

    ret = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if (ret != 0) {
        LOG(FATAL) << "init block sigpipe error!";
        return BITSTORE_ERR_SIGNAL_INIT;
    }
    LOG(INFO) << "configure, glog, semaphore init success!";

    return BITSTORE_OK;
}
