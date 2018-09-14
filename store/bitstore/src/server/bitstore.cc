#include <glog/logging.h>
#include <core/config.h>
#include <bitstore.pb.h>
#include <asyncio/blockdevice.h>
#include <asyncio/aio.h>
#include <asyncio/aiocontext.h>
#include <sys/time.h>
#include "bitstore.h"
#include "error/error.h"

BitStore::BitStore(bitstore_context_t *ctx)
{
    int ret = BITSTORE_OK;
    char errmsg[INT_LEN_256] = {0};

    this->bctx = ctx;
    block_device.bctx = ctx;
    ret = block_device_open(&block_device, bctx->config.aio.block_device);
    if (ret != BITSTORE_OK) {
        snprintf(errmsg, sizeof(errmsg), "block device %s open failed: %d",bctx->config.aio.block_device, ret);
        throw errmsg;
    }

    LOG(INFO) << "block device open success: " << bctx->config.aio.block_device;
}

BitStore::~BitStore()
{
}

grpc::Status BitStore::Put(grpc::ServerContext *context, grpc::ServerReader<bitstore::PutRequest> *reader, bitstore::PutResponse *response)
{
    int ret = BITSTORE_OK, count = 0;
    aio_context_t aioctx = {0};
    bitstore::PutRequest* reqs[100] = {0};
    struct timeval tv;
    uint64_t start_time = 0;

    gettimeofday(&tv,NULL);
    start_time = tv.tv_sec*1000000 + tv.tv_usec;
    std::cout << "entry put: " << start_time << std::endl;

    ret = aio_context_init(&aioctx);
    if (ret != BITSTORE_OK) {
        goto RESPONSE;
    }

    gettimeofday(&tv,NULL);
    std::cout << "aio context init: " << tv.tv_sec*1000000 + tv.tv_usec - start_time << std::endl;

    // TODO: 并发的接受和写入
    while (1) {
        bitstore::PutRequest *req = new bitstore::PutRequest;
        reqs[count++] = req;

        if (reader->Read(req)) {
            gettimeofday(&tv,NULL);
            std::cout << "grpc read: " << tv.tv_sec*1000000 + tv.tv_usec - start_time << std::endl;
            ret = block_device_aio_write(&block_device, &aioctx,
                                         req->offset(), req->length(),
                                         (char *)req->data().c_str());
            gettimeofday(&tv,NULL);
            std::cout << "aio write: " << tv.tv_sec*1000000 + tv.tv_usec - start_time << std::endl;
            if (ret != BITSTORE_OK) {
                LOG(ERROR) << "block asyncio write failed: " << ret;
                break;
            }
        } else {
            break;
        }
    }

    gettimeofday(&tv,NULL);
    std::cout << "aio write end: " << tv.tv_sec*1000000 + tv.tv_usec - start_time << std::endl;

    ret = block_device_aio_submit(&block_device, &aioctx);
    if (ret != BITSTORE_OK) {
        LOG(ERROR) << "block asyncio submit failed: " << ret;
    }

    // block wait for asyncio complete
    aio_context_wait(&aioctx);

    gettimeofday(&tv,NULL);
    std::cout << "aio wait: " << tv.tv_sec*1000000 + tv.tv_usec - start_time << std::endl;

    // sync data to disk
    fsync(block_device.fd_direct);

    ret = aio_context_return_value(&aioctx);
    if (ret < 0) {
        LOG(ERROR) << "block asyncio return negative value: " << ret;
    }
    aio_context_destroy(&aioctx);
    // TODO:优雅的释放new分配的内存
    for (int i = 0; i < count; i++) {
        delete reqs[i];
    }

    gettimeofday(&tv,NULL);
    std::cout << "exit: " << tv.tv_sec*1000000 + tv.tv_usec - start_time << std::endl;

    RESPONSE:
    response->set_errcode(ret);
    return grpc::Status::OK;
}

grpc::Status BitStore::Get(grpc::ServerContext *context, grpc::ServerReaderWriter<bitstore::GetResponse2, bitstore::GetRequest> *stream)
{
    int ret = BITSTORE_OK;
    aio_context_t aioctx = {0};
    bitstore::GetRequest req;
    bitstore::GetResponse2 resp;
    void *ppdata[100] = {0};
    int count = 0;

    // TODO: 释放read memalign分配的内存
    // TODO: 并发的下载和传输
    ret = aio_context_init(&aioctx);
    if (ret != BITSTORE_OK) {
        return grpc::Status::OK;
    }

    while (stream->Read(&req)) {
        ret = block_device_aio_read(&block_device, &aioctx, req.offset(), req.length(), &ppdata[count++]);
        if (ret != BITSTORE_OK) {
            LOG(ERROR) << "block asyncio read failed: " << ret;
            resp.set_errcode(ret);
            goto Exit;
        }
    }

    ret = block_device_aio_submit(&block_device, &aioctx);
    if (ret != BITSTORE_OK) {
        LOG(ERROR) << "block asyncio submit failed: " << ret;
    }
    // block wait for asyncio complete
    aio_context_wait(&aioctx);
    ret = aio_context_return_value(&aioctx);
    if (ret < 0) {
        LOG(ERROR) << "block asyncio return negative value: " << ret;
    }

    resp.set_errcode(0);
    for (int i = 0; i < count; i++) {
        resp.set_data((char *)ppdata[i]);
        stream->Write(resp);
    }

Exit:
    aio_context_destroy(&aioctx);
    return grpc::Status::OK;
}

void BitStore::Run()
{
    // start server
    grpc::ServerBuilder builder;
    builder.AddListeningPort(bctx->config.server.addr, grpc::InsecureServerCredentials());
    builder.SetMaxSendMessageSize(1024000);
    builder.SetMaxReceiveMessageSize(1024000);
    builder.RegisterService(this);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "BitStore Server Run With " << bctx->config.server.addr;
    server->Wait();
}
