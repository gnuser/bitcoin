数据存储
========

基本上要维护的有4部分数据
- **blocks/blk*.dat** bitcoin blocks数据，网络格式，直接存放在硬盘。当钱包搜索缺失的transactions时，构建不同的chains时，还有和其他节点同步时会用到。
- **blocks/index/*:** LevelDB数据库，包含了所有已知的blocks的元数据，增加索引，可以加快搜索block
- **chainstate**: LevelDB数据库，包含所有当前没被使用过的transaction输出和这些transaction的来源的元数据，用于验证新加入的blocks和transactions。理论上可以通过block数据重建(参考-reindex命令行参数)，但是会花费很长时间。即使没有这部分数据，你同样可以进行验证，但是这意味着你在每次使用transaction去验证他的输出时，需要搜索整个blocks。
- **blocks/rev.*.dat** 这里包含着"undo"的数据。可用于回滚chainstate，是重建所必须的数据

原始的block数据(blk*.dat)
------------------------
从网络获取的数据，直接保存
大概有128MB，切分成了16MB一块，以免碎片化。2015年10月，大概有365个block文件，总大小差不多45GB
每个block文件(比如blk1234.dat)都有一个对应的undo文件(rev1234.dat)，这个undo文件包含了一些必要数据，用于碰到分支事件时，从blockchain去掉blocks
信息保存在两个地方：
    - 基本信息保存在LevelDB的block索引对用的"f"开头的记录中(比如"fxxxxx", "xxxx"是4个数字)：
        - 文件中有多少个blocks
        - 文件大小(对应undo文件大小)
        - 文件中最低和最高的block
        - 时间戳(最早和最新的blocks)
    - 如何查找指定block的信息存放于"b"记录中：
        - 每个block都包含着一个指向硬盘文件的指针(文件number和偏移)
    
## 访问block数据文件
block文件访问通过：
- DiskBlockPos: 包含文件number和偏移，指向硬盘文件
- vInfoBlockFiles: BlockFileInfo对象vector。
    - 决定新的blocks是否放入当前文件，还是需要创建新的文件
    - 计算block和undo文件总的磁盘占用
    - 遍历block文件，查找可以修整的

只要接收到block文件并放入了AcceptBlock，就会尽快写入磁盘(实际写入函数WriteBlockToDisk)。注意访问block数据文件的代码和访问coin数据库的代码有重合，这里只是简单写入磁盘。


Block index(leveldb)
--------------------
包含所有已知的blocks索引，指向block磁盘的存储位置
已知的blocks，是最长chain的超集，包含了曾经接收和处理过但还没有成为激活chain的一部分的blocks。比如从激活链分支出去的孤儿blocks

## 术语
通常人们认为的"区块链"是指激活链(没被打断过，由X个blocks线性连接起来的，从创世block开始直到当前的block)，而代码中的区块链，除了激活链，还有大量的小分支链

- Block Tree
更好的描述了已知blocks集合，因为树形结构表示除了主链，还有很多分支。代码相关在src/txdb.h的CBlockTreeDB类中。

## 键值对
在实际的LevelDB数据库中，使用到的键值对定义：
```
   'b' + 32字节的block哈希值 -> 块索引记录. 每条记录保存内容:
       * 块头
       * 高度
       * transactions个数.
       * 验证方法
       * block文件存放于哪个文件，以及文件的哪个位置
       * undo文件存放于哪个文件，以及文件的哪个位置
```

```
  'f' + 4字节的文件编号 -> 文件信息记录. 每条记录保存内容:
       * 块文件中有多少个块
       * 块文件的大小 ($DATADIR/blocks/blkNNNNN.dat).
       * undo文件的大小 ($DATADIR/blocks/revNNNNN.dat).
       * 块的最低和最高高度
       * 块的最小和最大的时间戳
```

```
   'l' -> 4字节的文件编号: 最后一个块文件编号使用
```

```
   'R' -> 1字节布尔值('1'代表true): 是否正在重新索引
```

```
   'F' + 1字节 标志名称长度 + 标志名称字符串 -> 1字节布尔值 ('1'代表true, '0'代表false): 不同的标志可以打开和关闭，当前定义的标志有：
        * 'txindex': 是否transaction索引已经开启
```

```
    't' + 32字节的transaction哈希值 -> transaction索引记录。只在'txindex'开启时存在。每条记录保存内容：
       * transaction保存在哪个block中
       * transaction保存在block中的哪个部分
       * transaction保存在block中的偏移位置
```

## 数据访问层
数据库访问通过CBlockTreeDB, txdb.h
实例对象为一个全局变量pblocktree

_CBlockIndex_
当接收到头部数据的时候就直接创建了，而不需要等到完整的block。当收到多条头部数据的时候，所有的头部数据都存放入CBlockHeaders向量，然后会被检查。每个头部检查后都会创建一个CBlockIndex。

_CBlock/CBlcokHeader_
CBlock包含了当前block所有的transaction集合，数据存放于两个地方：
    - 完全的raw格式数据，在blk???.dat文件
    - 修整过的格式，放在UTXO数据库中

_加载block数据库到内存中_
在启动的时候，将加载整个数据库，参考LoadBlockIndexGuts(txdb.cpp)，会消耗几秒钟的时间
blocks('b'键值类型)将会加载到mapBlockIndex变量。 "mapblockIndex"是个unordered_map，包含了每个block的CBlockIndex，而不只是激活链
blocks('f'键值类型)加载到vInfoBlockFiles。

UTXO集合(chainstate leveldb)
----------------------------
"Ultraprune"的概念是为了减少以前的transaction的大小，只保留必要的transaction用于验证之后的transaction

## 术语
- UTXO = Unspent Transaction Out: transaction的输出，口语就是我们说的"币"，所以UTXO数据库就是指币数据库
- UTXO集合/币数据库/chainstate数据库：基本都指一个意思
- Provably Unspendable：如果币的scriptPubKey不满足条件，币就被证明为不可花费

## 键值对

```
   'c' + 32-byte transaction hash -> unspent transaction output record for that transaction. These records are only present for transactions that have at least one unspent output left. Each record stores:
       * The version of the transaction.
       * Whether the transaction was a coinbase or not.
       * Which height block contains the transaction.
       * Which outputs of that transaction are unspent.
       * The scriptPubKey and amount for those unspent outputs.


   'B' -> 32-byte block hash: the block hash up to which the database represents the unspent transaction outputs.
```

## 数据访问层
相比于block index,UTXO数据库更复杂，因为它决定着整个比特币系统的性能。在每一次交易发生时，所有的在UTXO数据库的数百万的币都会被检查和更改


原始的undo数据(rev*.dat)
------------------------
