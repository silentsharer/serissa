## Aim
实现一套兼容S3协议的开源分布式对象存储系统
## Features
### 兼容S3协议
* API兼容S3。
* 支持AWS V2、V4 认证。

### 对象存储
* 基于Facebook的Haystack论文。
* 采用Hashicorp Raft分布式一致性协议。
* 内部组件通信采用GRPC。
* 对外提供HTTP RESFful API形式访问。
* 提供完善的数据迁移方案。
* 提供完善的容错能力。
* 提供完善的灾备机制。
* 提供友好的运维工具或API。

### 低频存储
* 使用RS纠删码降低副本。