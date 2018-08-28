#ifndef BITSTORE_ERROR_H
#define BITSTORE_ERROR_H

#define BITSTORE_OK                                 0               // success
#define BITSTORE_ERR_FILE_LOAD                      -1000000        // load configure file failed
#define BITSTORE_ERR_FILE_OPEN                      -1000001        // open file failed
#define BITSTORE_ERR_FILE_STAT                      -1000002        // stat file failed
#define BITSTORE_ERR_PARAMETER                      -1000003        // parameter error
#define BITSTORE_ERR_SIGNAL_INIT                    -1000004        // signal init failed
#define BITSTORE_ERR_MALLOC                         -1000005        // malloc failed
#define BITSTORE_ERR_BLOCK_INIT_NOIO                -1000006        // no io to init block
#define BITSTORE_ERR_THREAD_INIT                    -1001000        // thread init failed
#define BITSTORE_ERR_THREAD_CREATE                  -1001001        // thread create failed
#define BITSTORE_ERR_THREAD_MUTEX_INIT              -1001002        // mutex init failed
#define BITSTORE_ERR_THREAD_MUTEX_DESTROY           -1001003        // mutex destroy failed
#define BITSTORE_ERR_THREAD_MUTEX_LOCK              -1001004        // mutex lock failed
#define BITSTORE_ERR_THREAD_MUTEX_UNLOCK            -1001005        // mutex unlock failed
#define BITSTORE_ERR_THREAD_COND_INIT               -1001006        // cond init failed
#define BITSTORE_ERR_THREAD_COND_DESTROY            -1001007        // cond destroy failed
#define BITSTORE_ERR_THREAD_COND_WAIT               -1001008        // cond wait failed
#define BITSTORE_ERR_THREAD_COND_SIGNAL             -1001009        // cond signal failed
#define BITSTORE_ERR_THREAD_COND_BROADCAST          -1001010        // cond broadcast failed
#define BITSTORE_ERR_AIO_CONTEXT_LIST_FULL          -1002000        // aio context list full
#define BITSTORE_ERR_AIO_EIO                        -1002001        // aio eio

#endif // BITSTORE_ERROR_H
