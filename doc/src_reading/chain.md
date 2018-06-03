Chain
=====

CChain类
-------
这个类主要存放索引
成员变量只有一个vChain，一个元素类型为CBlockIndex的vector
```
    std::vector<CBlockIndex*> vChain;
```

### Genesis函数
获取第一个索引
### Tip函数
获取最后一个索引
### operator[]函数
获取指定位置(参数名为nHeight，代表高度)的索引
### operator==函数
比较两个CChain对象，实现只比较了最后一个索引
```
    /** Compare two chains efficiently. */
    friend bool operator==(const CChain &a, const CChain &b) {
        return a.vChain.size() == b.vChain.size() &&
               a.vChain[a.vChain.size() - 1] == b.vChain[b.vChain.size() - 1];
    }
```
### Contains(CBlockIndex *pindex)函数
检查是否包含索引
### Next(const CBlockIndex *pindex)函数
获取指定索引的下一个索引，指定的索引必须存在于链中
### Height函数
获取链的高度，从0开始计数
### SetTip函数
从尾部开始往前循环设置整条链
```c++
void CChain::SetTip(CBlockIndex *pindex) {
    // 如果传入NULL，则清除所有的元素，相当于初始化
    if (pindex == NULL) {
        vChain.clear();
        return;
    }
    // 根据给定的索引高度，申请容器容量个数，因为高度是从0计数，所以要+1
    vChain.resize(pindex->nHeight + 1);
    // 循环设置整条链
    while (pindex && vChain[pindex->nHeight] != pindex) {
        vChain[pindex->nHeight] = pindex;
        pindex = pindex->pprev;
    }
}
```
### GetLocator函数
获取索引对应的CBlockLocator
```c++
CBlockLocator CChain::GetLocator(const CBlockIndex *pindex) const {
    int nStep = 1;
    std::vector<uint256> vHave;
    vHave.reserve(32);
    // 如果传入的是NULL，获取最后一个索引
    if (!pindex)
        pindex = Tip();
    while (pindex) {
        vHave.push_back(pindex->GetBlockHash());
        // Stop when we have added the genesis block.
        // 如果已经到了创世块，则跳出循环
        if (pindex->nHeight == 0)
            break;
        // Exponentially larger steps back, plus the genesis block.
        // 否则以指数级别的间隔往前推移(nStep变化：1 -> 2 -> 4 -> 8 ...)，但不能超过创世块的位置
        int nHeight = std::max(pindex->nHeight - nStep, 0);
        // 如果之前的索引是存在的
        if (Contains(pindex)) {
            // Use O(1) CChain index if possible.
            // 获取对应高度的索引
            pindex = (*this)[nHeight];
        } else {
            // Otherwise, use O(log n) skiplist.
            // 否则，获取对应高度的索引的祖先索引
            pindex = pindex->GetAncestor(nHeight);
        }
        // 如果已经找到的索引超过10个，则每找到一个都指数级别间隔往前寻找，加快速度
        if (vHave.size() > 10)
            nStep *= 2;
    }

    return CBlockLocator(vHave);
}
```
### FindFork函数
找到当前索引是从哪个链的哪个索引开始生成的
```c++
const CBlockIndex *CChain::FindFork(const CBlockIndex *pindex) const {
    if (pindex == NULL) {
        return NULL;
    }
    // 如果指定的索引高度超过链的高度，则获取末尾索引的祖先索引
    if (pindex->nHeight > Height())
        pindex = pindex->GetAncestor(Height());
    // 循环往前推移，直到找到一个不在当前链的索引
    while (pindex && !Contains(pindex))
        pindex = pindex->pprev;
    return pindex;
}
```
### FindEarliestAtLeast函数
获取给定时间的索引
```c++
CBlockIndex* CChain::FindEarliestAtLeast(int64_t nTime) const
{
    std::vector<CBlockIndex*>::const_iterator lower = std::lower_bound(vChain.begin(), vChain.end(), nTime,
        [](CBlockIndex* pBlock, const int64_t& time) -> bool { return pBlock->GetBlockTimeMax() < time; });
    return (lower == vChain.end() ? NULL : *lower);
}
```







