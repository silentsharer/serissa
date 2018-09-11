#ifndef BITSTORE_BITSTORE_H
#define BITSTORE_BITSTORE_H

#include <iostream>
#include <grpcpp/grpcpp.h>
#include <bitstore.grpc.pb.h>
#include <bitstore.pb.h>
#include "core/config.h"
#include "aio/blockdevice.h"

class BitStore final : public bitstore::BitStore::Service
{
public:
    BitStore(bitstore_context_t *ctx);
    ~BitStore();

    grpc::Status Put(grpc::ServerContext* context, grpc::ServerReader<bitstore::PutRequest>* reader, bitstore::PutResponse* response) override;
    grpc::Status Get(grpc::ServerContext* context, grpc::ServerReaderWriter<bitstore::GetResponse2, bitstore::GetRequest>* stream) override;
    void Run();
public:
    bitstore_context_t *bctx;
    block_device_t block_device;
};

#endif //BITSTORE_BITSTORE_H
