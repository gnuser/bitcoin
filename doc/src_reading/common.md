基础结构
========

哈希(Hashes)
------
比特币的地址通常由一串字符串表示，这一串字符串就是通过SHA-256等算法生成的。
double-SHA-256算法举例:
```
hello
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824 (first round of sha-256)
9595c9df90075148eb06860365df33584b75bff782a510c6cd4883a419833d50 (second round of sha-256)
```

RIPEMD-160算法用来生成稍短长度的哈希串，比如比特币地址
```
hello
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824 (first round is sha-256)
b6a9c8c230722b7c748331a8b450f05566dc7d0f (with ripemd-160)
```

默克尔树(Merkle Tree)
---------------------

用来存放哈希值的二叉树。使用了double-SHA-256算法
当构造树的行数据时（非root节点），树的节点个数为偶数
首先构造最底层的树节点，使用块里面排好序的哈希交易字节流
然后往上构造节点，每个节点都是由两个子节点哈希值拼接起来的
直到构造都根节点

举例：

```
d1 = dhash(a)
d2 = dhash(b)
d3 = dhash(c)
d4 = dhash(c)            # a, b, c are 3. that's an odd number, so we take the c twice

d5 = dhash(d1 concat d2)
d6 = dhash(d3 concat d4)

d7 = dhash(d5 concat d6)
```

dhash函数即为double-SHA-256，即为两次SHA-256计算
```
dhash(a) = sha256(sha256(a))
```

d7为整棵树的根节点

## 注意大小端格式

签名
----

Bitcoin使用了ECDSA对交易进行签名


交易认证
-------

交易通过加密签名记录将Bitcoins的归属权重新赋值。交易的inputs-record，引用自之前所有者，outputs-records决定了将Bitcoin转移给新的所有者。然后再作为下一次交易的inputs-records

每个input必须有一个加密数字签名，用来解锁之前交易的资产。只有资产的所有者使用私钥才能生成有效的签名，这样就可以保证资产只能被拥有者使用。

每个output决定了资产的去向

在一次交易中，所有的inputs之和必须大于等于所有的outputs之和。如果是大于，多出的部分会被认为是交易手续费，用来奖励矿工。

一个特殊的交易，称为基币交易，没有inputs。是被矿工创造的，每一个块都有一个基币。每一个新块的产生都会产生一部分Bitcoin（前210,000个块是50个BTC每块）。块的第一次交易就是把这些币授权给矿工。除了新创建的币，基币交易也会用于在同一块中产生的交易手续费。基币交易可以将整个收益全都给单个Bitcoin地址，或者拆分到多个地址中，和其他的交易一样。基币交易里总是包含了同一区块的所有的区块奖励和所有的交易手续费。
0区块的基币交易是不可以用来消费的。

大多数的Bitcoin的outputs都

