区块链
======

当节点初始化时，加载块索引数据库。这意味着整个块树，而不只是激活链。
块索引的"验证状态"：获取当前块的验证进度和祖先块
    - VALID_HEADER = 1
    - VALID_TREE = 2
    - VALID_TRANSACTIONS = 3
    - VALID_CHAIN = 4
    - VALID_SCRIPTS = 5    
需要注意的存储在块索引的两个变量：
    nTx:交易个数
        nTx > 0以为块有一个状态时VALID_TRANSACTION。
    nChainTx:块链的交易个数，直到并包括当前块
        nChainTx > 0代表整个链是VALID_TRANSACTION状态

关键变量
--------

## mapBlockIndex(map<block_hash, CBlockIndex*>)

## mapBlocksUnlinked

## setBlockIndexCandidates

## pindexBestHeader

## chainActive(vector<CBlockIndex*>)



