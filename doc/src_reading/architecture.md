Bitcoin架构
===========

基础的消息结构，序列化以及反序列化处理
基础结构的上层是网络组件，用于管理sockets，发现peer，并且处理和分发消息
然后是脚本引擎，签名验证组件，签名组件
最重要的操作是需要验证和转发的，比如satoshi客户端的transaction验证，blockchain和内存池的管理，发现瞎搞的peers，这些目前主要实现在main.h/main.cpp

比较简洁的架构图如下：
```
  Validating transactions; Managing blockchain, mempool, and peers.
                   | 
      Scripting engine / Signatures
                   | 
             Network layer
                   | 
             P2P Messages
```

加上对应的代码部分：
```
  Validating transactions; Managing blockchain, mempool, peers  (Consensus and Policy code)
                   | 
      Scripting engine / Signatures (Consensus code)
                   | 
             Network layer  (P2P code) 
                   | 
             P2P Messages
```

