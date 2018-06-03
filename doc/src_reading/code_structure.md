源代码结构
==========

├── addrdb.cpp
├── addrdb.h
├── addrman.cpp
├── addrman.h
├── amount.h
├── arith_uint256.cpp
├── arith_uint256.h
├── base58.cpp
├── base58.h
├── bench
│   ├── base58.cpp
│   ├── bench_bitcoin.cpp
│   ├── bench.cpp
│   ├── bench.h
│   ├── ccoins_caching.cpp
│   ├── checkblock.cpp
│   ├── checkqueue.cpp
│   ├── coin_selection.cpp
│   ├── crypto_hash.cpp
│   ├── data
│   ├── Examples.cpp
│   ├── lockedpool.cpp
│   ├── mempool_eviction.cpp
│   ├── perf.cpp
│   ├── perf.h
│   ├── prevector_destructor.cpp
│   ├── rollingbloom.cpp
│   └── verify_script.cpp
├── bitcoin-cli.cpp
├── bitcoin-cli-res.rc
├── bitcoind.cpp
├── bitcoind-res.rc
├── bitcoin-tx.cpp
├── bitcoin-tx-res.rc
├── blockencodings.cpp
├── blockencodings.h
├── bloom.cpp
├── bloom.h
```
block以及blockchain定义
├── chain.cpp
├── chain.h
├── chainparamsbase.cpp
├── chainparamsbase.h
├── chainparams.cpp
├── chainparams.h
├── chainparamsseeds.h
```
├── checkpoints.cpp
├── checkpoints.h
├── checkqueue.h
├── clientversion.cpp
├── clientversion.h
```
├── coins.cpp
├── coins.h
```
```
一些兼容性处理
├── compat
│   ├── byteswap.h
│   ├── endian.h
│   ├── glibc_compat.cpp
│   ├── glibc_sanity.cpp
│   ├── glibcxx_sanity.cpp
│   ├── sanity.h
│   └── strnlen.cpp
```
├── compat.h
├── compressor.cpp
├── compressor.h
```
编译相关
├── config
│   └── bitcoin-config.h.in
```
```
├── consensus # 验证blocks和transactions
│   ├── consensus.h
│   ├── merkle.cpp
│   ├── merkle.h
│   ├── params.h
│   ├── tx_verify.cpp
│   ├── tx_verify.h
│   └── validation.h
```
├── core_io.h
├── core_memusage.h
├── core_read.cpp
├── core_write.cpp
```
加密算法
├── crypto
│   ├── aes.cpp
│   ├── aes.h
│   ├── chacha20.cpp
│   ├── chacha20.h
│   ├── common.h
│   ├── ctaes
│   ├── hmac_sha256.cpp
│   ├── hmac_sha256.h
│   ├── hmac_sha512.cpp
│   ├── hmac_sha512.h
│   ├── ripemd160.cpp
│   ├── ripemd160.h
│   ├── sha1.cpp
│   ├── sha1.h
│   ├── sha256.cpp
│   ├── sha256.h
│   ├── sha512.cpp
│   └── sha512.h
```
├── cuckoocache.h
├── dbwrapper.cpp
├── dbwrapper.h
├── fs.cpp
├── fs.h
├── hash.cpp
├── hash.h
├── httprpc.cpp
├── httprpc.h
├── httpserver.cpp
├── httpserver.h
├── indirectmap.h
├── init.cpp # 初始化流程
├── init.h
├── key.cpp
├── key.h
├── keystore.cpp
├── keystore.h
```
leveldb库
├── leveldb
│   ├── AUTHORS
│   ├── build_detect_platform
│   ├── CONTRIBUTING.md
│   ├── db
│   ├── doc
│   ├── helpers
│   ├── include
│   ├── issues
│   ├── LICENSE
│   ├── Makefile
│   ├── NEWS
│   ├── port
│   ├── README.md
│   ├── table
│   ├── TODO
│   ├── util
│   └── WINDOWS.md
```
├── limitedmap.h
├── Makefile.am
├── Makefile.bench.include
├── Makefile.in
├── Makefile.leveldb.include
├── Makefile.qt.include
├── Makefile.qttest.include
├── Makefile.test.include
├── memusage.h
├── merkleblock.cpp
├── merkleblock.h
```
挖矿相关
├── miner.cpp
├── miner.h
```
```
这些文件实现了网络部分，包括peer连接等
├── netaddress.cpp
├── netaddress.h
├── netbase.cpp
├── netbase.h
├── net.cpp
├── net.h
├── netmessagemaker.h
├── net_processing.cpp
├── net_processing.h
```
├── noui.cpp
├── noui.h
```
编译相关
├── obj
├── obj-test
```
```
├── policy # 特定node规则实现
│   ├── feerate.cpp
│   ├── feerate.h
│   ├── fees.cpp
│   ├── fees.h
│   ├── policy.cpp
│   ├── policy.h
│   ├── rbf.cpp
│   └── rbf.h
```
├── pow.cpp
├── pow.h
├── prevector.h
```
基础数据结构定义
├── primitives
│   ├── block.cpp
│   ├── block.h
│   ├── transaction.cpp
│   └── transaction.h
```
├── protocol.cpp
├── protocol.h
├── pubkey.cpp
├── pubkey.h
```
qt界面部分
├── qt
│   ├── addressbookpage.cpp
│   ├── addressbookpage.h
│   ├── addresstablemodel.cpp
│   ├── addresstablemodel.h
│   ├── askpassphrasedialog.cpp
│   ├── askpassphrasedialog.h
│   ├── bantablemodel.cpp
│   ├── bantablemodel.h
│   ├── bitcoinaddressvalidator.cpp
│   ├── bitcoinaddressvalidator.h
│   ├── bitcoinamountfield.cpp
│   ├── bitcoinamountfield.h
│   ├── bitcoin.cpp
│   ├── bitcoingui.cpp
│   ├── bitcoingui.h
│   ├── bitcoin_locale.qrc
│   ├── bitcoin.qrc
│   ├── bitcoinstrings.cpp
│   ├── bitcoinunits.cpp
│   ├── bitcoinunits.h
│   ├── callback.h
│   ├── clientmodel.cpp
│   ├── clientmodel.h
│   ├── coincontroldialog.cpp
│   ├── coincontroldialog.h
│   ├── coincontroltreewidget.cpp
│   ├── coincontroltreewidget.h
│   ├── csvmodelwriter.cpp
│   ├── csvmodelwriter.h
│   ├── editaddressdialog.cpp
│   ├── editaddressdialog.h
│   ├── forms
│   ├── guiconstants.h
│   ├── guiutil.cpp
│   ├── guiutil.h
│   ├── intro.cpp
│   ├── intro.h
│   ├── locale
│   ├── macdockiconhandler.h
│   ├── macdockiconhandler.mm
│   ├── macnotificationhandler.h
│   ├── macnotificationhandler.mm
│   ├── Makefile
│   ├── modaloverlay.cpp
│   ├── modaloverlay.h
│   ├── networkstyle.cpp
│   ├── networkstyle.h
│   ├── notificator.cpp
│   ├── notificator.h
│   ├── openuridialog.cpp
│   ├── openuridialog.h
│   ├── optionsdialog.cpp
│   ├── optionsdialog.h
│   ├── optionsmodel.cpp
│   ├── optionsmodel.h
│   ├── overviewpage.cpp
│   ├── overviewpage.h
│   ├── paymentrequestplus.cpp
│   ├── paymentrequestplus.h
│   ├── paymentrequest.proto
│   ├── paymentserver.cpp
│   ├── paymentserver.h
│   ├── peertablemodel.cpp
│   ├── peertablemodel.h
│   ├── platformstyle.cpp
│   ├── platformstyle.h
│   ├── qvalidatedlineedit.cpp
│   ├── qvalidatedlineedit.h
│   ├── qvaluecombobox.cpp
│   ├── qvaluecombobox.h
│   ├── receivecoinsdialog.cpp
│   ├── receivecoinsdialog.h
│   ├── receiverequestdialog.cpp
│   ├── receiverequestdialog.h
│   ├── recentrequeststablemodel.cpp
│   ├── recentrequeststablemodel.h
│   ├── res
│   ├── rpcconsole.cpp
│   ├── rpcconsole.h
│   ├── sendcoinsdialog.cpp
│   ├── sendcoinsdialog.h
│   ├── sendcoinsentry.cpp
│   ├── sendcoinsentry.h
│   ├── signverifymessagedialog.cpp
│   ├── signverifymessagedialog.h
│   ├── splashscreen.cpp
│   ├── splashscreen.h
│   ├── test
│   ├── trafficgraphwidget.cpp
│   ├── trafficgraphwidget.h
│   ├── transactiondesc.cpp
│   ├── transactiondescdialog.cpp
│   ├── transactiondescdialog.h
│   ├── transactiondesc.h
│   ├── transactionfilterproxy.cpp
│   ├── transactionfilterproxy.h
│   ├── transactionrecord.cpp
│   ├── transactionrecord.h
│   ├── transactiontablemodel.cpp
│   ├── transactiontablemodel.h
│   ├── transactionview.cpp
│   ├── transactionview.h
│   ├── utilitydialog.cpp
│   ├── utilitydialog.h
│   ├── walletframe.cpp
│   ├── walletframe.h
│   ├── walletmodel.cpp
│   ├── walletmodel.h
│   ├── walletmodeltransaction.cpp
│   ├── walletmodeltransaction.h
│   ├── walletview.cpp
│   ├── walletview.h
│   ├── winshutdownmonitor.cpp
│   └── winshutdownmonitor.h
```
├── random.cpp
├── random.h
├── rest.cpp
├── reverselock.h
├── rpc
│   ├── blockchain.cpp
│   ├── blockchain.h
│   ├── client.cpp
│   ├── client.h
│   ├── mining.cpp
│   ├── misc.cpp
│   ├── net.cpp
│   ├── protocol.cpp
│   ├── protocol.h
│   ├── rawtransaction.cpp
│   ├── register.h
│   ├── server.cpp
│   └── server.h
├── scheduler.cpp
├── scheduler.h
```
脚本引擎
├── script
│   ├── bitcoinconsensus.cpp
│   ├── bitcoinconsensus.h
│   ├── interpreter.cpp
│   ├── interpreter.h
│   ├── ismine.cpp
│   ├── ismine.h
│   ├── script.cpp
│   ├── script_error.cpp
│   ├── script_error.h
│   ├── script.h
│   ├── sigcache.cpp
│   ├── sigcache.h
│   ├── sign.cpp
│   ├── sign.h
│   ├── standard.cpp
│   └── standard.h
```
```
ECDSA加密
├── secp256k1
│   ├── aclocal.m4
│   ├── autogen.sh
│   ├── autom4te.cache
│   ├── build-aux
│   ├── configure
│   ├── configure.ac
│   ├── contrib
│   ├── COPYING
│   ├── include
│   ├── libsecp256k1.pc.in
│   ├── Makefile.am
│   ├── Makefile.in
│   ├── obj
│   ├── README.md
│   ├── sage
│   ├── src
│   └── TODO
```
├── serialize.h
├── streams.h
├── support
│   ├── allocators
│   ├── cleanse.cpp
│   ├── cleanse.h
│   ├── events.h
│   ├── lockedpool.cpp
│   └── lockedpool.h
├── sync.cpp
├── sync.h
```
测试相关，使用boost单元测试框架
├── test
│   ├── addrman_tests.cpp
│   ├── allocator_tests.cpp
│   ├── amount_tests.cpp
│   ├── arith_uint256_tests.cpp
│   ├── base32_tests.cpp
│   ├── base58_tests.cpp
│   ├── base64_tests.cpp
│   ├── bip32_tests.cpp
│   ├── blockencodings_tests.cpp
│   ├── bloom_tests.cpp
│   ├── bswap_tests.cpp
│   ├── checkqueue_tests.cpp
│   ├── coins_tests.cpp
│   ├── compress_tests.cpp
│   ├── crypto_tests.cpp
│   ├── cuckoocache_tests.cpp
│   ├── data
│   ├── dbwrapper_tests.cpp
│   ├── DoS_tests.cpp
│   ├── getarg_tests.cpp
│   ├── hash_tests.cpp
│   ├── key_tests.cpp
│   ├── limitedmap_tests.cpp
│   ├── main_tests.cpp
│   ├── Makefile
│   ├── mempool_tests.cpp
│   ├── merkle_tests.cpp
│   ├── miner_tests.cpp
│   ├── multisig_tests.cpp
│   ├── netbase_tests.cpp
│   ├── net_tests.cpp
│   ├── pmt_tests.cpp
│   ├── policyestimator_tests.cpp
│   ├── pow_tests.cpp
│   ├── prevector_tests.cpp
│   ├── raii_event_tests.cpp
│   ├── random_tests.cpp
│   ├── README.md
│   ├── reverselock_tests.cpp
│   ├── rpc_tests.cpp
│   ├── sanity_tests.cpp
│   ├── scheduler_tests.cpp
│   ├── scriptnum10.h
│   ├── scriptnum_tests.cpp
│   ├── script_P2SH_tests.cpp
│   ├── script_tests.cpp
│   ├── serialize_tests.cpp
│   ├── sighash_tests.cpp
│   ├── sigopcount_tests.cpp
│   ├── skiplist_tests.cpp
│   ├── streams_tests.cpp
│   ├── test_bitcoin.cpp
│   ├── test_bitcoin_fuzzy.cpp
│   ├── test_bitcoin.h
│   ├── test_bitcoin_main.cpp
│   ├── testutil.cpp
│   ├── testutil.h
│   ├── timedata_tests.cpp
│   ├── torcontrol_tests.cpp
│   ├── transaction_tests.cpp
│   ├── txvalidationcache_tests.cpp
│   ├── uint256_tests.cpp
│   ├── univalue_tests.cpp
│   ├── util_tests.cpp
│   └── versionbits_tests.cpp
```
├── threadinterrupt.cpp
├── threadinterrupt.h
├── threadsafety.h
├── timedata.cpp
├── timedata.h
├── tinyformat.h
├── torcontrol.cpp
├── torcontrol.h
├── txdb.cpp
├── txdb.h
├── txmempool.cpp
├── txmempool.h
├── ui_interface.cpp
├── ui_interface.h
├── uint256.cpp
├── uint256.h
├── undo.h
```
通用value对象库
├── univalue
│   ├── aclocal.m4
│   ├── autogen.sh
│   ├── autom4te.cache
│   ├── build-aux
│   ├── configure
│   ├── configure.ac
│   ├── COPYING
│   ├── gen
│   ├── include
│   ├── lib
│   ├── Makefile.am
│   ├── Makefile.in
│   ├── pc
│   ├── README
│   ├── test
│   ├── TODO
│   ├── univalue-config.h.in
│   └── univalue-config.h.in~
```
├── util.cpp
├── util.h
├── utilmoneystr.cpp
├── utilmoneystr.h
├── utilstrencodings.cpp
├── utilstrencodings.h
├── utiltime.cpp
├── utiltime.h
├── validation.cpp
├── validation.h
├── validationinterface.cpp
├── validationinterface.h
├── versionbits.cpp
├── versionbits.h
├── version.h
├── wallet
│   ├── coincontrol.h
│   ├── crypter.cpp
│   ├── crypter.h
│   ├── db.cpp
│   ├── db.h
│   ├── feebumper.cpp
│   ├── feebumper.h
│   ├── rpcdump.cpp
│   ├── rpcwallet.cpp
│   ├── rpcwallet.h
│   ├── test
│   ├── wallet.cpp
│   ├── walletdb.cpp
│   ├── walletdb.h
│   └── wallet.h
├── warnings.cpp
├── warnings.h
```
zmq异步消息库
└── zmq
    ├── zmqabstractnotifier.cpp
    ├── zmqabstractnotifier.h
    ├── zmqconfig.h
    ├── zmqnotificationinterface.cpp
    ├── zmqnotificationinterface.h
    ├── zmqpublishnotifier.cpp
    └── zmqpublishnotifier.h
```