#include <glog/logging.h>
#include <core/config.h>
#include <bitstore.pb.h>
#include <asyncio/blockdevice.h>
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

    ret = aio_context_init(&aioctx);
    if (ret != BITSTORE_OK) {
        goto RESPONSE;
    }

    while (1) {
        bitstore::PutRequest *req = new bitstore::PutRequest;
        reqs[count++] = req;

        if (reader->Read(req)) {
            ret = block_device_aio_write(&block_device, &aioctx,
                                         req->offset(), req->length(),
                                         (char *)req->data().c_str());
            if (ret != BITSTORE_OK) {
                LOG(ERROR) << "block asyncio write failed: " << ret;
                break;
            }
        } else {
            break;
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
    aio_context_destroy(&aioctx);
    // TODO:优雅的释放new分配的内存
    for (int i = 0; i < count; i++) {
        delete reqs[i];
    }

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

    ret = aio_context_init(&aioctx);
    if (ret != BITSTORE_OK) {
        return grpc::Status::OK;
    }

    while (stream->Read(&req)) {
        void *data = NULL;
        ret = block_device_aio_read(&block_device, &aioctx, req.offset(), req.length(), &data);
        if (ret != BITSTORE_OK) {
            LOG(ERROR) << "block asyncio read failed: " << ret;
            break;
        }

        // block wait for asyncio complete
        aio_context_wait(&aioctx);
        ret = aio_context_return_value(&aioctx);
        if (ret < 0) {
            LOG(ERROR) << "block asyncio return negative value: " << ret;
        }

        resp.set_errcode(ret);
        resp.set_data((char*)data);

        stream->Write(resp);
        free(data);
    }

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
