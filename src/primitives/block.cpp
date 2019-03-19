// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "crypto/common.h"

uint256 CBlockHeader::GetHash() const
{
    /** 实现: 
    /** Compute the 256-bit hash of an object's serialization.
        template<typename T>
        uint256 SerializeHash(const T& obj, int nType=SER_GETHASH, int nVersion=PROTOCOL_VERSION)
        {
            CHashWriter ss(nType, nVersion);
            ss << obj;
            return ss.GetHash();
        }

        CHashWriter方法重写了<<操作符:
        // CHashWriter作为stream,将obj序列化
        template<typename T>
        CHashWriter& operator<<(const T& obj) {
            // Serialize to this stream
            ::Serialize(*this, obj);
            return (*this);
        }
        // 序列化模板,会调用对象的Serialize方法
        template<typename Stream, typename T>
        inline void Serialize(Stream& os, const T& a)
        {
            a.Serialize(os);
        }
        // 因为添加了序列化宏
        #define ADD_SERIALIZE_METHODS                                         \
            template<typename Stream>                                         \
            void Serialize(Stream& s) const {                                 \
                NCONST_PTR(this)->SerializationOp(s, CSerActionSerialize());  \
            }                                                                 \
        // 这里会调用CBlockHeader的SerializationOp方法
        // 而具体的成员类型会调用模板
        template<typename Stream> inline void Serialize(Stream& s, char a    ) { ser_writedata8(s, a); } // TODO Get rid of bare char
        template<typename Stream> inline void Serialize(Stream& s, int8_t a  ) { ser_writedata8(s, a); }
        // ser_writedata函数将最终调用stream的write函数
        template<typename Stream> inline void ser_writedata8(Stream &s, uint8_t obj)
        {
            s.write((char*)&obj, 1);
        }
        // 也就是这里的CHashWrite类的write函数
        // 这里会将字节内容写到ctx对象中,用来构造hash
        void write(const char *pch, size_t size) {
           ctx.Write((const unsigned char*)pch, size);
        }
     */
    return SerializeHash(*this);
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i]->ToString() << "\n";
    }
    return s.str();
}

int64_t GetBlockWeight(const CBlock& block)
{
    // This implements the weight = (stripped_size * 4) + witness_size formula,
    // using only serialization with and without witness data. As witness_size
    // is equal to total_size - stripped_size, this formula is identical to:
    // weight = (stripped_size * 3) + total_size.
    return ::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS) * (WITNESS_SCALE_FACTOR - 1) + ::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION);
}
