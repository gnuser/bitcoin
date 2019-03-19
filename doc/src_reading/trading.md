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
    // 获取参数1，转出地址
    CBitcoinAddress address(request.params[0].get_str());
    // 检查地址是否有效
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Bitcoin address");

    // Amount， 获取参数2，转出数量
    CAmount nAmount = AmountFromValue(request.params[1]);
    if (nAmount <= 0)
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid amount for send");

    // Wallet comments
    CWalletTx wtx;
    // 获取参数3，备注 
    if (request.params.size() > 2 && !request.params[2].isNull() && !request.params[2].get_str().empty())
        wtx.mapValue["comment"] = request.params[2].get_str();
    // 获取参数4
    if (request.params.size() > 3 && !request.params[3].isNull() && !request.params[3].get_str().empty())
        wtx.mapValue["to"]      = request.params[3].get_str();

    bool fSubtractFeeFromAmount = false;
    // 获取参数5， 是否从转出数量里扣除手续费
    if (request.params.size() > 4)
        fSubtractFeeFromAmount = request.params[4].get_bool();
    // 检查钱包是否是解锁状态
    EnsureWalletIsUnlocked(pwallet);
    // 转出币
    SendMoney(pwallet, address.Get(), nAmount, fSubtractFeeFromAmount, wtx);

    return wtx.GetHash().GetHex();
}
```

转币的处理函数实现
```cpp
static void SendMoney(CWallet * const pwallet, const CTxDestination &address, CAmount nValue, bool fSubtractFeeFromAmount, CWalletTx& wtxNew)
{
	// 获取钱包余额
    CAmount curBalance = pwallet->GetBalance();

    // Check amount， 检查amount，不能小于等于0
    if (nValue <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount");
    // 不能大于剩余金额
    if (nValue > curBalance)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");
    // 如果钱包不支持广播交易功能，则抛出异常
    if (pwallet->GetBroadcastTransactions() && !g_connman) {
        throw JSONRPCError(RPC_CLIENT_P2P_DISABLED, "Error: Peer-to-peer functionality missing or disabled");
    }

    // Parse Bitcoin address，解析转出地址，获取对应脚本对象
    CScript scriptPubKey = GetScriptForDestination(address);

    // Create and send the transaction， 构造交易
    CReserveKey reservekey(pwallet);
    CAmount nFeeRequired;
    std::string strError;
    // 存放收款人的信息
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
    // 包含脚本公钥，转出数量，是否从转出数量扣除手续费
    CRecipient recipient = {scriptPubKey, nValue, fSubtractFeeFromAmount};
    vecSend.push_back(recipient);
    // 创建交易
    if (!pwallet->CreateTransaction(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strError)) {
    	// 检查是否转出数量+手续费超过余额，超过的话，提醒所需的手续费数量
        if (!fSubtractFeeFromAmount && nValue + nFeeRequired > curBalance)
            strError = strprintf("Error: This transaction requires a transaction fee of at least %s", FormatMoney(nFeeRequired));
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }
    // 发送交易
    CValidationState state;
    if (!pwallet->CommitTransaction(wtxNew, reservekey, g_connman.get(), state)) {
    	// 获取发送错误的信息
        strError = strprintf("Error: The transaction was rejected! Reason given: %s", state.GetRejectReason());
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }
}
```

创建交易代码位置src/wallet/wallet.cpp

```cpp
bool CWallet::CreateTransaction(const std::vector<CRecipient>& vecSend, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRet,
                                int& nChangePosInOut, std::string& strFailReason, const CCoinControl* coinControl, bool sign)
{
    CAmount nValue = 0;
    int nChangePosRequest = nChangePosInOut;
    unsigned int nSubtractFeeFromAmount = 0;
    // 遍历收款人信息
    for (const auto& recipient : vecSend)
    {
    	// 检查发送数量
        if (nValue < 0 || recipient.nAmount < 0)
        {
            strFailReason = _("Transaction amounts must not be negative");
            return false;
        }
        // 累计总量
        nValue += recipient.nAmount;
        // 累计转出数量扣除手续费的次数
        if (recipient.fSubtractFeeFromAmount)
            nSubtractFeeFromAmount++;
    }
    // 检查信息列表是否为空
    if (vecSend.empty())
    {
        strFailReason = _("Transaction must have at least one recipient");
        return false;
    }
    // 
    wtxNew.fTimeReceivedIsTxTime = true;
    wtxNew.BindWallet(this);
    CMutableTransaction txNew;
    // 不鼓励手续费削减
    // Discourage fee sniping.
    // 
    // 对于大矿工来说，交易能放在最优的区块和内存池中的代价，
    // For a large miner the value of the transactions in the best block and
    // 可能会超过故意尝试挖两个区块来独立当前最优区块的成本。
    // the mempool can exceed the cost of deliberately attempting to mine two
    // 通过设置nLockTime，
    // blocks to orphan the current best block. By setting nLockTime such that
    // 使得只能下一个区块才能包含交易，我们不推荐这样的使用，
    // only the next block can include the transaction, we discourage this
    // 因为区块高度的限制和有限的区块容量，
    // practice as the height restricted and limited blocksize gives miners
    // 让矿工考虑手续费削减更少的选择来完成此次攻击。
    // considering fee sniping fewer options for pulling off this attack.
    //
    // 一个简单的方法来思考这个问题，就是从钱包的角度，
    // A simple way to think about this is from the wallet's point of view we
    // 我们总是希望区块往前移动。
    // always want the blockchain to move forward. By setting nLockTime this
    // 通过设置nLockTime这种方法，我们基本上是在声明我们只想要这个交易出现
    // way we're basically making the statement that we only want this
    // 在下一个区块；我们不想潜在地鼓励通过允许交易出现在比下一个区块更低的高度，
    // transaction to appear in the next block; we don't want to potentially
    // 来使最优链分叉
    // encourage reorgs by allowing transactions to appear at lower heights
    // than the next block in forks of the best chain.
    //
    // 当然，补贴已经足够高，并且交易量也足够低，所以手续费削减已经不是问题了
    // Of course, the subsidy is high enough, and transaction volume low
    // 但是通过实现了一个修复当前时间，
    // enough, that fee sniping isn't a problem yet, but by implementing a fix
    // 我们能确保代码不会被写成是，
    // now we ensure code won't be written that makes assumptions about
    // 假设使nLockTime排除掉修复的后面的时间
    // nLockTime that preclude a fix later.
    // 获取到活跃链的高度
    txNew.nLockTime = chainActive.Height();

    // 第二，偶尔随机取一个非常往前的nLockTime，
    // Secondly occasionally randomly pick a nLockTime even further back, so
    // 以使得交易被延迟到因为任何原因而签名之后
    // that transactions that are delayed after signing for whatever reason,
    // 比如高延迟的复杂网络环境和一些更隐私的聚合币的实现
    // e.g. high-latency mix networks and some CoinJoin implementations, have
    // better privacy.
    if (GetRandInt(10) == 0)
        txNew.nLockTime = std::max(0, (int)txNew.nLockTime - GetRandInt(100));

    // nLockTime必须小于等于活跃链的高度
    assert(txNew.nLockTime <= (unsigned int)chainActive.Height());
    // 必须小于LOCKTIME_THRESHOLD
    assert(txNew.nLockTime < LOCKTIME_THRESHOLD);
    // 计算手续费
    FeeCalculation feeCalc;
    // 存放交易占用字节
    unsigned int nBytes;
    {
    	// 输入的utxo列表
        std::set<CInputCoin> setCoins;
        // 锁住钱包
        LOCK2(cs_main, cs_wallet);
        {
        	// 输出列表
            std::vector<COutput> vAvailableCoins;
            // 获取有效的输出列表
            AvailableCoins(vAvailableCoins, true, coinControl);

            nFeeRet = 0;
            // Start with no fee and loop until there is enough fee
            while (true)
            {
                nChangePosInOut = nChangePosRequest;
                txNew.vin.clear();
                txNew.vout.clear();
                wtxNew.fFromMe = true;
                bool fFirst = true;

                CAmount nValueToSelect = nValue;
                // 如果不再需要扣除手续费，则将已经累计的手续费加入总的转出额
                if (nSubtractFeeFromAmount == 0)
                    nValueToSelect += nFeeRet;
                // vouts to the payees
                // 遍历输出信息列表
                for (const auto& recipient : vecSend)
                {
                	// 构建TxOut对象
                    CTxOut txout(recipient.nAmount, recipient.scriptPubKey);
                    // 计算手续费
                    if (recipient.fSubtractFeeFromAmount)
                    {
                    	// 平摊手续费在每一个要求转出包含手续费的交易上
                        txout.nValue -= nFeeRet / nSubtractFeeFromAmount; // Subtract fee equally from each selected recipient
                        // 第一个收款人将扣掉除不尽的剩余手续费
                        if (fFirst) // first receiver pays the remainder not divisible by output count
                        {
                            fFirst = false;
                            txout.nValue -= nFeeRet % nSubtractFeeFromAmount;
                        }
                    }
                    // 是否是尘埃级别的手续费，手续费设置得太小是不允许的
                    if (IsDust(txout, ::dustRelayFee))
                    {
                        if (recipient.fSubtractFeeFromAmount && nFeeRet > 0)
                        {
                            if (txout.nValue < 0)
                                strFailReason = _("The transaction amount is too small to pay the fee");
                            else
                                strFailReason = _("The transaction amount is too small to send after the fee has been deducted");
                        }
                        else
                            strFailReason = _("Transaction amount too small");
                        return false;
                    }
                    txNew.vout.push_back(txout);
                }

                // Choose coins to use
                // 查找utxo
                CAmount nValueIn = 0;
                setCoins.clear();
                // 获取可用的输入
                if (!SelectCoins(vAvailableCoins, nValueToSelect, setCoins, nValueIn, coinControl))
                {
                    strFailReason = _("Insufficient funds");
                    return false;
                }
                // 计算总输入和总输出的差值
                const CAmount nChange = nValueIn - nValueToSelect;
                if (nChange > 0)
                {
                    // Fill a vout to ourself， 把剩余值打回自己账号
                    // TODO: pass in scriptChange instead of reservekey so
                    // change transaction isn't always pay-to-bitcoin-address
                    CScript scriptChange;

                    // coin control: send change to custom address
                    // 将剩余值转入自定义的地址
                    if (coinControl && !boost::get<CNoDestination>(&coinControl->destChange))
                        scriptChange = GetScriptForDestination(coinControl->destChange);
                    // 或者生成一个新的地址来存放
                    // no coin control: send change to newly generated address
                    else
                    {
                    	// 用新地址可能产生的问题，以及复用老地址的问题说明
                        // Note: We use a new key here to keep it from being obvious which side is the change.
                        //  The drawback is that by not reusing a previous key, the change may be lost if a
                        //  backup is restored, if the backup doesn't have the new private key for the change.
                        //  If we reused the old key, it would be possible to add code to look for and
                        //  rediscover unknown transactions that were written with keys of ours to recover
                        //  post-backup change.

                        // Reserve a new key pair from key pool， 从密钥池里面取一个新的密钥
                        CPubKey vchPubKey;
                        bool ret;
                        ret = reservekey.GetReservedKey(vchPubKey, true);
                        if (!ret)
                        {
                            strFailReason = _("Keypool ran out, please call keypoolrefill first");
                            return false;
                        }

                        scriptChange = GetScriptForDestination(vchPubKey.GetID());
                    }

                    CTxOut newTxOut(nChange, scriptChange);

                    // Never create dust outputs; if we would, just
                    // add the dust to the fee.
                    // 如果剩余值太小，直接作为手续费
                    if (IsDust(newTxOut, ::dustRelayFee))
                    {
                        nChangePosInOut = -1;
                        nFeeRet += nChange;
                        reservekey.ReturnKey();
                    }
                    else
                    {
                        if (nChangePosInOut == -1)
                        {
                            // Insert change txn at random position:
                            nChangePosInOut = GetRandInt(txNew.vout.size()+1);
                        }
                        else if ((unsigned int)nChangePosInOut > txNew.vout.size())
                        {
                            strFailReason = _("Change index out of range");
                            return false;
                        }

                        std::vector<CTxOut>::iterator position = txNew.vout.begin()+nChangePosInOut;
                        txNew.vout.insert(position, newTxOut);
                    }
                } else {
                    reservekey.ReturnKey();
                    nChangePosInOut = -1;
                }

                // Fill vin
                //
                // Note how the sequence number is set to non-maxint so that
                // the nLockTime set above actually works.
                //
                // BIP125 defines opt-in RBF as any nSequence < maxint-1, so
                // we use the highest possible value in that range (maxint-2)
                // to avoid conflicting with other possible uses of nSequence,
                // and in the spirit of "smallest possible change from prior
                // behavior."
                // 设置sequence
                bool rbf = coinControl ? coinControl->signalRbf : fWalletRbf;
                const uint32_t nSequence = rbf ? MAX_BIP125_RBF_SEQUENCE : (std::numeric_limits<unsigned int>::max() - 1);
                for (const auto& coin : setCoins)
                    txNew.vin.push_back(CTxIn(coin.outpoint,CScript(),
                                              nSequence));

                // Fill in dummy signatures for fee calculation.
                // 填充假签名，用来计算手续费，因为手续费只跟字节数相关，只要填入一样长度的字节，就可以计算了
                if (!DummySignTx(txNew, setCoins)) {
                    strFailReason = _("Signing transaction failed");
                    return false;
                }
                // 计算交易的字节数
                nBytes = GetVirtualTransactionSize(txNew);

                // Remove scriptSigs to eliminate the fee calculation dummy signatures
                for (auto& vin : txNew.vin) {
                    vin.scriptSig = CScript();
                    vin.scriptWitness.SetNull();
                }

                // Allow to override the default confirmation target over the CoinControl instance
                int currentConfirmationTarget = nTxConfirmTarget;
                if (coinControl && coinControl->nConfirmTarget > 0)
                    currentConfirmationTarget = coinControl->nConfirmTarget;

                CAmount nFeeNeeded = GetMinimumFee(nBytes, currentConfirmationTarget, ::mempool, ::feeEstimator, &feeCalc);
                if (coinControl && coinControl->fOverrideFeeRate)
                    nFeeNeeded = coinControl->nFeeRate.GetFee(nBytes);

                // If we made it here and we aren't even able to meet the relay fee on the next pass, give up
                // because we must be at the maximum allowed fee.
                if (nFeeNeeded < ::minRelayTxFee.GetFee(nBytes))
                {
                    strFailReason = _("Transaction too large for fee policy");
                    return false;
                }

                if (nFeeRet >= nFeeNeeded) {
                    // Reduce fee to only the needed amount if we have change
                    // output to increase.  This prevents potential overpayment
                    // in fees if the coins selected to meet nFeeNeeded result
                    // in a transaction that requires less fee than the prior
                    // iteration.
                    // TODO: The case where nSubtractFeeFromAmount > 0 remains
                    // to be addressed because it requires returning the fee to
                    // the payees and not the change output.
                    // TODO: The case where there is no change output remains
                    // to be addressed so we avoid creating too small an output.
                    if (nFeeRet > nFeeNeeded && nChangePosInOut != -1 && nSubtractFeeFromAmount == 0) {
                        CAmount extraFeePaid = nFeeRet - nFeeNeeded;
                        std::vector<CTxOut>::iterator change_position = txNew.vout.begin()+nChangePosInOut;
                        change_position->nValue += extraFeePaid;
                        nFeeRet -= extraFeePaid;
                    }
                    break; // Done, enough fee included.
                }

                // Try to reduce change to include necessary fee
                if (nChangePosInOut != -1 && nSubtractFeeFromAmount == 0) {
                    CAmount additionalFeeNeeded = nFeeNeeded - nFeeRet;
                    std::vector<CTxOut>::iterator change_position = txNew.vout.begin()+nChangePosInOut;
                    // Only reduce change if remaining amount is still a large enough output.
                    if (change_position->nValue >= MIN_FINAL_CHANGE + additionalFeeNeeded) {
                        change_position->nValue -= additionalFeeNeeded;
                        nFeeRet += additionalFeeNeeded;
                        break; // Done, able to increase fee from change
                    }
                }

                // Include more fee and try again.
                nFeeRet = nFeeNeeded;
                continue;
            }
        }

        if (sign)
        {
            CTransaction txNewConst(txNew);
            int nIn = 0;
            for (const auto& coin : setCoins)
            {
                const CScript& scriptPubKey = coin.txout.scriptPubKey;
                SignatureData sigdata;

                if (!ProduceSignature(TransactionSignatureCreator(this, &txNewConst, nIn, coin.txout.nValue, SIGHASH_ALL), scriptPubKey, sigdata))
                {
                    strFailReason = _("Signing transaction failed");
                    return false;
                } else {
                    UpdateTransaction(txNew, nIn, sigdata);
                }

                nIn++;
            }
        }

        // Embed the constructed transaction data in wtxNew.
        wtxNew.SetTx(MakeTransactionRef(std::move(txNew)));

        // Limit size
        if (GetTransactionWeight(wtxNew) >= MAX_STANDARD_TX_WEIGHT)
        {
            strFailReason = _("Transaction too large");
            return false;
        }
    }

    if (GetBoolArg("-walletrejectlongchains", DEFAULT_WALLET_REJECT_LONG_CHAINS)) {
        // Lastly, ensure this tx will pass the mempool's chain limits
        LockPoints lp;
        CTxMemPoolEntry entry(wtxNew.tx, 0, 0, 0, false, 0, lp);
        CTxMemPool::setEntries setAncestors;
        size_t nLimitAncestors = GetArg("-limitancestorcount", DEFAULT_ANCESTOR_LIMIT);
        size_t nLimitAncestorSize = GetArg("-limitancestorsize", DEFAULT_ANCESTOR_SIZE_LIMIT)*1000;
        size_t nLimitDescendants = GetArg("-limitdescendantcount", DEFAULT_DESCENDANT_LIMIT);
        size_t nLimitDescendantSize = GetArg("-limitdescendantsize", DEFAULT_DESCENDANT_SIZE_LIMIT)*1000;
        std::string errString;
        if (!mempool.CalculateMemPoolAncestors(entry, setAncestors, nLimitAncestors, nLimitAncestorSize, nLimitDescendants, nLimitDescendantSize, errString)) {
            strFailReason = _("Transaction has too long of a mempool chain");
            return false;
        }
    }

    LogPrintf("Fee Calculation: Fee:%d Bytes:%u Tgt:%d (requested %d) Reason:\"%s\" Decay %.5f: Estimation: (%g - %g) %.2f%% %.1f/(%.1f %d mem %.1f out) Fail: (%g - %g) %.2f%% %.1f/(%.1f %d mem %.1f out)\n",
              nFeeRet, nBytes, feeCalc.returnedTarget, feeCalc.desiredTarget, StringForFeeReason(feeCalc.reason), feeCalc.est.decay,
              feeCalc.est.pass.start, feeCalc.est.pass.end,
              100 * feeCalc.est.pass.withinTarget / (feeCalc.est.pass.totalConfirmed + feeCalc.est.pass.inMempool + feeCalc.est.pass.leftMempool),
              feeCalc.est.pass.withinTarget, feeCalc.est.pass.totalConfirmed, feeCalc.est.pass.inMempool, feeCalc.est.pass.leftMempool,
              feeCalc.est.fail.start, feeCalc.est.fail.end,
              100 * feeCalc.est.fail.withinTarget / (feeCalc.est.fail.totalConfirmed + feeCalc.est.fail.inMempool + feeCalc.est.fail.leftMempool),
              feeCalc.est.fail.withinTarget, feeCalc.est.fail.totalConfirmed, feeCalc.est.fail.inMempool, feeCalc.est.fail.leftMempool);
    return true;
}

```









