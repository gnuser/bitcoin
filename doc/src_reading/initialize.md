初始化以及启动流程
==================

参考https://en.bitcoin.it/wiki/Bitcoin_Core_0.11_(ch_3):_Initialization_and_Startup

程序入口
--------
程序入口，main函数在bitcoind.cpp

main函数只有3行代码
```c++
    SetupEnvironment();
    noui_connect();
    return (AppInit(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE);
```

- **SetupEnvironment** (设置areana个数，设置程序locale)
- **noui_connect** (设置信号处理函数)
- **AppInit** (初始化以及运行)

AppInit也定义在bitcoind.cpp，流程如下：
- 解析命令行参数
- 打开数据文件目录
- 读取config文件
- os相关的设置(dump，socket初始化，umask，信号处理)
- 初始化及检查设置各种参数，(某些参数需要另外的参数一起设置)
- 设置一些全局变量
- 初始化sanity检查
- 创建子进程(如果是后台运行)
- 进入AppInitMain函数，执行后续流程

AppInitMain函数在init.cpp文件，流程如下：
- 锁住数据文件目录
- 创建pid文件
- 检查压缩debug.log文件
- 打开debug.log文件
- 初始化签名缓存
- 创建n个线程，用于脚本检查
- 开启任务调度线程
- 以warmup模式开启rpc服务器
- 如果开启了钱包，验证钱包
- 网络初始化
- 加载block chain
	- 计算cache size
	- 加载blockchain到mapBlockIndex(将会花费10-20秒)
	- UTXO设置
- 加载钱包	
- 数据目录维护
- 导入blocks
- 启动node
- Finished

初始化完成后，回到bitcoind.cpp
调用 **WaitForShutdown** 进入无限循环，等待退出事件
**Shutdown**函数需要保证文件写入和缓存保存，关闭RPC服务，关闭node，注销掉信号处理等等
