P2P
===

本篇介绍P2P网络相关，包括发现和连接其他节点并且与之通信(比如传递blocks和transactions的消息)，禁掉一些瞎搞的节点，bitcoin自定义了一套P2P消息协议，大多数的代码都在net.h/net.cpp

管理peers的数据结构
-------------------

任何时候，当前节点都连接着多个其他节点，称为"peers"。默认情况，一个节点连接着最多8个外部的节点(称为"outbound peers")，并且允许最多125个其他节点来连接我们(称为"inbound peers")，每个节点就是一个CNode对象
CNode有很多属性可以设置，大多跟底层数据流处理有关：

- nServices
    + 是一个bitmap类型，它定义节点的服务类型
- fClient
    + 是否是SPV节点
- fOneShot
- fInbound
    + 是否是inbound节点
- fWhiteListed
    + 是否是whitelisted节点
- setInventoryKnown
- vSendMsg
- vRecvMsg

查找并且连接peer
----------------

## 地址管理

```
设计目标：
  * 地址表在内存中维护，并且异步存储整个表在peers.dat文件
  * 保证没有攻击者可以填充整个地址表

To that end: 
  * Addresses are organized into buckets.
    * Addresses that have not yet been tried go into 1024 "new" buckets.
    * Addresses of nodes that are known to be accessible go into 256 "tried" buckets.
    * Bucket selection is based on cryptographic hashing, using a randomly-generated 256-bit key, which should not be observable by adversaries.
    * Several indexes are kept for high performance. Defining DEBUG_ADDRMAN will introduce frequent (and expensive) consistency checks for the entire data structure.
```

## 时间戳

发现peer
--------

可以通过多种方式：
    - 地址数据库(peers.dat)
    - 用户自定义(-addnode和-connect)
    - DNS seeding
    - Hard-coded seeds
    - 从其他peers

1. 地址数据库
    启动时从peers.dat文件加载，当第一次运行的时候，此文件是没东西的。
2. 用户自定义
    - 支持指定多个节点
    - 地址初始时的时间戳为0，所以不会被公布
    - -connect参数指定的IP地址不会加入peers.dat文件
    - -addnode参数指定的IP地址会在开始就被使用，但节点会很快去寻找别的peers
3. DNS seeding
    当peers.dat为空时被使用
    这时节点会发起DNS请求去查找其他peers的IP地址
    被查找到的地址时间戳为0，以免被公布
4. Hard-coded节点
    如果DNS seeding失败，就会使用硬编码地址，这是最后的措施
5. 从其他节点获取("getaddr"和"addr"消息)
    通常发出"getaddr"请求，会得到"addr"消息
    也有可能是主动发过来的"addr"消息，因为节点会向外公布自己的地址
当收到"version"消息的时候，节点会发出"getaddr"消息

当收到"addr"消息时的处理：
当收到"getaddr"消息时的处理：





