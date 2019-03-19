// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCK_H
#define BITCOIN_PRIMITIVES_BLOCK_H

#include "primitives/transaction.h"
#include "serialize.h"
#include "uint256.h"

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
/** 节点搜集新的交易打包到区块,通过hash计算构造一棵hash树,
 * 通过寻找nonce值来获取区块hash值,以满足pow的需求.
 * 当获取了pow的随机数需求,将广播区块给每个人,然后该区块会被加入到链.
 * 区块的第一个交易比较特殊,会生成新币,用来奖励给区块节点的创造者
 */
class CBlockHeader
{
public:
    // header
    int32_t nVersion;       // 版本
    uint256 hashPrevBlock;  // 上一个区块的hash值
    uint256 hashMerkleRoot; // merkle树根的hash值
    uint32_t nTime;         // 时间戳
    uint32_t nBits;         // pow难度
    uint32_t nNonce;        // 满足pow的随机数

    CBlockHeader()          // 初始化数据结构
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;  // 序列化宏, 添加了此宏后,只需要实现SerializationOp函数,该类就可以被序列化以及反序列化

    // 序列化操作
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(this->nVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
    }

    // 清空所有字段
    void SetNull()
    {
        nVersion = 0;
        hashPrevBlock.SetNull();
        hashMerkleRoot.SetNull();
        nTime = 0;
        nBits = 0;
        nNonce = 0;
    }

    // 如果nBits为0,则区块为空
    bool IsNull() const
    {
        return (nBits == 0);
    }

    // 获取hash值
    uint256 GetHash() const;

    // 获取区块时间
    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }
};


class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransactionRef> vtx;   // 交易列表

    // memory only
    mutable bool fChecked;  // 是否被验证过

    CBlock()    // 初始化结构
    {
        SetNull();
    }

    CBlock(const CBlockHeader &header) // 复制
    {
        SetNull();
        *((CBlockHeader*)this) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(*(CBlockHeader*)this); // 将对象传入,会调用对象的Serialise方法,这里会调用CBlockHeader的SerializationOp方法
        READWRITE(vtx); // vector序列化
    }

    void SetNull()
    {
        CBlockHeader::SetNull();
        vtx.clear();
        fChecked = false;
    }

    // 获取区块头结构
    CBlockHeader GetBlockHeader() const
    {
        CBlockHeader block;
        block.nVersion       = nVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        return block;
    }

    std::string ToString() const;
};

/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
/** 描述区块链中另外一个节点的位置,
 * 如果其他节点没有同样的分支,可以找到一个最近的公共主干,
 * 这个位置越靠后,则其越在分叉前
 */
struct CBlockLocator
{
    std::vector<uint256> vHave;

    CBlockLocator() {}

    CBlockLocator(const std::vector<uint256>& vHaveIn) : vHave(vHaveIn) {}

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        int nVersion = s.GetVersion();
        if (!(s.GetType() & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    }

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull() const
    {
        return vHave.empty();
    }
};

/** Compute the consensus-critical block weight (see BIP 141). */
int64_t GetBlockWeight(const CBlock& tx);

#endif // BITCOIN_PRIMITIVES_BLOCK_H
