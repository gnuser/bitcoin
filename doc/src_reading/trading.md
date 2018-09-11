---
layout: post
title: 吃透比特币源码-交易
categories: 吃透比特币源码
---

## 目标
分析最重要的交易涉及到的代码，理解交易的产生过程，验证过程。
可以说交易是比特币的核心，其他所有的模块都是围绕着交易来做的，
所以一旦交易吃透，将对整个比特币的理解更加深刻。






* 1. show me the navigator
{:toc}

## 切入口
如果直接翻代码，估计涉及的比较多，所以我从RPC入口切入，这样从应用的角度，会更直接

# 转币命令
```shell
$ bitcoin-cli help sendtoaddress
参数:
1. 转出地址
2. 转出数量
3. 备注
4. 转出人或者组织的名称备注
5. 是否从转出数量里面扣除手续费

使用举例:
$ bitcoin-cli sendtoaddress "1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd" 0.1 "buy pizza" "by codeskr" true
```

第5个参数比较有意思，也就是使用者如果有1个BTC在钱包，你可以不用关心手续费是多少，直接将1个BTC作为转出数量，全部转走，
否则你如果自己算手续费，就要计算扣除手续费后的数量作为转出数量

* 代码实现
位置在src\wallet\rpcwallet.cpp

```cpp
UniValue sendtoaddress(const JSONRPCRequest& request)
{
	// 获取钱包对象
    CWallet * const pwallet = GetWalletForJSONRPCRequest(request);
    // 检查pwallet是否有效
    if (!EnsureWalletIsAvailable(pwallet, request.fHelp)) {
        return NullUniValue;
    }

    // 检查参数个数，不符合则输出帮助信息
    if (request.fHelp || request.params.size() < 2 || request.params.size() > 5)
        throw std::runtime_error(
            "sendtoaddress \"address\" amount ( \"comment\" \"comment_to\" subtractfeefromamount )\n"
            "\nSend an amount to a given address.\n"
            + HelpRequiringPassphrase(pwallet) +
            "\nArguments:\n"
            "1. \"address\"            (string, required) The bitcoin address to send to.\n"
            "2. \"amount\"             (numeric or string, required) The amount in " + CURRENCY_UNIT + " to send. eg 0.1\n"
            "3. \"comment\"            (string, optional) A comment used to store what the transaction is for. \n"
            "                             This is not part of the transaction, just kept in your wallet.\n"
            "4. \"comment_to\"         (string, optional) A comment to store the name of the person or organization \n"
            "                             to which you're sending the transaction. This is not part of the \n"
            "                             transaction, just kept in your wallet.\n"
            "5. subtractfeefromamount  (boolean, optional, default=false) The fee will be deducted from the amount being sent.\n"
            "                             The recipient will receive less bitcoins than you enter in the amount field.\n"
            "\nResult:\n"
            "\"txid\"                  (string) The transaction id.\n"
            "\nExamples:\n"
            + HelpExampleCli("sendtoaddress", "\"1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\" 0.1")
            + HelpExampleCli("sendtoaddress", "\"1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\" 0.1 \"donation\" \"seans outpost\"")
            + HelpExampleCli("sendtoaddress", "\"1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\" 0.1 \"\" \"\" true")
            + HelpExampleRpc("sendtoaddress", "\"1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\", 0.1, \"donation\", \"seans outpost\"")
        );
    // 锁住钱包
    LOCK2(cs_main, pwallet->cs_wallet);
    // 获取第一个参数，转出地址
    CBitcoinAddress address(request.params[0].get_str());
    // 检查地址是否有效
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Bitcoin address");

    // Amount
    CAmount nAmount = AmountFromValue(request.params[1]);
    if (nAmount <= 0)
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid amount for send");

    // Wallet comments
    CWalletTx wtx;
    if (request.params.size() > 2 && !request.params[2].isNull() && !request.params[2].get_str().empty())
        wtx.mapValue["comment"] = request.params[2].get_str();
    if (request.params.size() > 3 && !request.params[3].isNull() && !request.params[3].get_str().empty())
        wtx.mapValue["to"]      = request.params[3].get_str();

    bool fSubtractFeeFromAmount = false;
    if (request.params.size() > 4)
        fSubtractFeeFromAmount = request.params[4].get_bool();

    EnsureWalletIsUnlocked(pwallet);

    SendMoney(pwallet, address.Get(), nAmount, fSubtractFeeFromAmount, wtx);

    return wtx.GetHash().GetHex();
}
```




