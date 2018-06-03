序列化和反序列化
================

比特币的交易指令在发送网络前，会先经过序列化处理。什么是序列化呢？简单点说，序列化是将内部定义的数据结构转换为字节流，而反序列化正好相反，是把字节流转换为内部定义的数据结构。

让我们直接来使用一下，从简单的基本类型到复杂的类结构。

## 基本类型测试
```c++
BOOST_AUTO_TEST_CASE(base_data_type)
{
  CDataStream ss(SER_DISK, 0);
  int age = 10;
  int age_output;
  ss << age;
  ss >> age_output;
  std::cout << "deserilized int: " << age_output << std::endl;

  char flag = 'Y';
  char flag_output;
  ss << flag;
  ss >> flag_output;
  std::cout << "deserilized char: " << flag_output << std::endl;

  float hp = 5.2;
  float hp_output;
  ss << hp;
  ss >> hp_output;
  std::cout << "deserilized float: " << hp_output << std::endl;

  std::vector<int> members (5, 1);
  std::vector<int> members_output;
  ss << members;
  ss >> members_output;
  std::cout << "deserilized vector count: " << members_output.size() << std::endl;
}
```

## 类结构测试
```c++
class Simple
{
protected:
  int age;
  char flag;
  float hp;
  std::vector<int> members;
  std::string name;
public:
  Simple() = default;
  Simple(int age_in, char flag_in, float hp_in, std::vector<int> members_in, std::string name_in)
    : age(age_in), flag(flag_in), hp(hp_in), members(members_in), name(name_in)
  {
  }
  ADD_SERIALIZE_METHODS;
  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(age);
    READWRITE(flag);
    READWRITE(hp);
    READWRITE(name);
  }

  std::string& getName()
  {
    return name;
  }
};

BOOST_AUTO_TEST_CASE(class_type)
{
  CDataStream ss(SER_DISK, 0);
  std::vector<int> members (5, 1);
  std::string name = "gnuser";
  Simple simple(10, 'Y', 5.2, members, name);
  Simple simple_output;
  ss << simple;
  ss >> simple_output;
  std::cout << "deserilized class name: " << simple_output.getName() << std::endl;
}
```


CDataStream
-----------
比特币序列化实现的核心的就是CDataStream类，其在streams.h文件中定义
字面上意思是个数据流，可以往里面填充数据，也可以导出数据。


### 操作符<<方法
实际上调用的是Serialize方法

```c++
template<typename T>
CDataStream& operator<<(const T& obj)
{
    // Serialize to this stream
    ::Serialize(*this, obj);
    return (*this);
}
```

### Serialize方法
一系列针对各种数据类型的模板函数，定义在serialize.h文件

- 基本类型的实现
```c++
template<typename Stream> inline void Serialize(Stream& s, char a    ) { ser_writedata8(s, a); } // TODO Get rid of bare char
template<typename Stream> inline void Serialize(Stream& s, int8_t a  ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int16_t a ) { ser_writedata16(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint16_t a) { ser_writedata16(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int32_t a ) { ser_writedata32(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint32_t a) { ser_writedata32(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int64_t a ) { ser_writedata64(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint64_t a) { ser_writedata64(s, a); }
template<typename Stream> inline void Serialize(Stream& s, float a   ) { ser_writedata32(s, ser_float_to_uint32(a)); }
template<typename Stream> inline void Serialize(Stream& s, double a  ) { ser_writedata64(s, ser_double_to_uint64(a)); }
template<typename Stream> inline void Serialize(Stream& s, bool a)    { char f=a; ser_writedata8(s, f); }
template<typename Stream> inline void Unserialize(Stream& s, bool& a) { char f=ser_readdata8(s); a=f; }
```

- 字符串类型，会将该类型实际占用的字节数提前写入数据流，然后写入字符串内容
```
```c++
/**
 * string
 */
template<typename Stream, typename C>
void Serialize(Stream& os, const std::basic_string<C>& str)
{
    WriteCompactSize(os, str.size());
    if (!str.empty())
        os.write((char*)&str[0], str.size() * sizeof(str[0]));
}
```
- 容器类型，会把容器元素个数先写入数据流，然后循环写入容器内元素
```c++
/**
 * vector
 */
template<typename Stream, typename T, typename A>
void Serialize_impl(Stream& os, const std::vector<T, A>& v, const unsigned char&)
{
    WriteCompactSize(os, v.size());
    if (!v.empty())
        os.write((char*)&v[0], v.size() * sizeof(T));
}

template<typename Stream, typename T, typename A, typename V>
void Serialize_impl(Stream& os, const std::vector<T, A>& v, const V&)
{
    WriteCompactSize(os, v.size());
    for (typename std::vector<T, A>::const_iterator vi = v.begin(); vi != v.end(); ++vi)
        ::Serialize(os, (*vi));
}

template<typename Stream, typename T, typename A>
inline void Serialize(Stream& os, const std::vector<T, A>& v)
{
    Serialize_impl(os, v, T());
}

/**
 * pair
 */
template<typename Stream, typename K, typename T>
void Serialize(Stream& os, const std::pair<K, T>& item)
{
    Serialize(os, item.first);
    Serialize(os, item.second);
}


/**
 * map
 */
template<typename Stream, typename K, typename T, typename Pred, typename A>
void Serialize(Stream& os, const std::map<K, T, Pred, A>& m)
{
    WriteCompactSize(os, m.size());
    for (typename std::map<K, T, Pred, A>::const_iterator mi = m.begin(); mi != m.end(); ++mi)
        Serialize(os, (*mi));
}


/**
 * set
 */
template<typename Stream, typename K, typename Pred, typename A>
void Serialize(Stream& os, const std::set<K, Pred, A>& m)
{
    WriteCompactSize(os, m.size());
    for (typename std::set<K, Pred, A>::const_iterator it = m.begin(); it != m.end(); ++it)
        Serialize(os, (*it));
}
```

- 指针类型
```c++
/**
 * unique_ptr
 */
template<typename Stream, typename T> void
Serialize(Stream& os, const std::unique_ptr<const T>& p)
{
    Serialize(os, *p);
}

/**
 * shared_ptr
 */
template<typename Stream, typename T> void
Serialize(Stream& os, const std::shared_ptr<const T>& p)
{
    Serialize(os, *p);
}
```

- 非特化类型实现，直接调用对应类型的Serialize方法
```c++
/**
 * If none of the specialized versions above matched, default to calling member function.
 */
template<typename Stream, typename T>
inline void Serialize(Stream& os, const T& a)
{
    a.Serialize(os);
}
```

最终调用的字节写入函数其实是write函数
```c++
template<typename Stream> inline void ser_writedata8(Stream &s, uint8_t obj)
{
    s.write((char*)&obj, 1);
}
```

### write函数的实现

直接调用了内部成员vch的insert方法
```c++
void write(const char* pch, size_t nSize)
{
    // Write to the end of the buffer
    vch.insert(vch.end(), pch, pch + nSize);
}
```

### vch的定义

vch是一个自定义allocator(zero_after_free_allocator)的vector容易
```c++
typedef CSerializeData vector_type;
vector_type vch;
```

zero_after_free_allocator的定义
```c++
// Byte-vector that clears its contents before deletion.
typedef std::vector<char, zero_after_free_allocator<char> > CSerializeData;
```

而这个zero_after_free_allocator只为了一件事
就是在释放内存前，清理掉原有值，也是为了安全吧
```c++
void deallocate(T* p, std::size_t n)
{
    if (p != NULL)
        memory_cleanse(p, sizeof(T) * n);
    std::allocator<T>::deallocate(p, n);
}
```

### 写入序列化类型的字节大小函数WriteCompactSize实现

- < 253字节
    + 以单字节存入
- <= unsigned short最大数值
    + 先写入253单字节标志位
    + 再以双字节存入
- <= unsigned int最大数值
    + 先写入254单字节标志位
    + 再以4字节存入
- 其他情况
    + 先写入255单字节标志位
    + 再以8字节存入
```c++
inline void WriteCompactSize(CSizeComputer& os, uint64_t nSize);
template<typename Stream>
void WriteCompactSize(Stream& os, uint64_t nSize)
{
    if (nSize < 253)
    {
        ser_writedata8(os, nSize);
    }
    else if (nSize <= std::numeric_limits<unsigned short>::max())
    {
        ser_writedata8(os, 253);
        ser_writedata16(os, nSize);
    }
    else if (nSize <= std::numeric_limits<unsigned int>::max())
    {
        ser_writedata8(os, 254);
        ser_writedata32(os, nSize);
    }
    else
    {
        ser_writedata8(os, 255);
        ser_writedata64(os, nSize);
    }
    return;
}

inline void WriteCompactSize(CSizeComputer &s, uint64_t nSize)
{
    s.seek(GetSizeOfCompactSize(nSize));
}
```

反序列化
--------

### 操作符>>方法
实际上调用的是Unserialize方法

```c++
template<typename T>
CDataStream& operator>>(T& obj)
{
    // Unserialize from this stream
    ::Unserialize(*this, obj);
    return (*this);
}
```

Unserialize的模板函数跟Serialize的一一对应

- 普通类型比较简单，直接读出类型对应内存字节数
- 字符串类型，需要先读出字符串大小，再读出整个字符串
```c++
template<typename Stream, typename C>
void Unserialize(Stream& is, std::basic_string<C>& str)
{
    unsigned int nSize = ReadCompactSize(is);
    str.resize(nSize);
    if (nSize != 0)
        is.read((char*)&str[0], nSize * sizeof(str[0]));
}
```
- 容器类型
    - vector类型处理比较特殊，对每次申请vector容量时做了限制，防止内存申请过多导致内存不足
    - 其他容器类型，通过解析出容器内的元素个数，逐一获取插入到参数指定的容器地址，代码就不贴了
```c++
template<typename Stream, typename T, typename A>
void Unserialize_impl(Stream& is, std::vector<T, A>& v, const unsigned char&)
{
    // Limit size per read so bogus size value won't cause out of memory
    v.clear();
    unsigned int nSize = ReadCompactSize(is);
    unsigned int i = 0;
    while (i < nSize)
    {
        unsigned int blk = std::min(nSize - i, (unsigned int)(1 + 4999999 / sizeof(T)));
        v.resize(i + blk);
        is.read((char*)&v[i], blk * sizeof(T));
        i += blk;
    }
}

template<typename Stream, typename T, typename A, typename V>
void Unserialize_impl(Stream& is, std::vector<T, A>& v, const V&)
{
    v.clear();
    unsigned int nSize = ReadCompactSize(is);
    unsigned int i = 0;
    unsigned int nMid = 0;
    while (nMid < nSize)
    {
        nMid += 5000000 / sizeof(T);
        if (nMid > nSize)
            nMid = nSize;
        v.resize(nMid);
        for (; i < nMid; i++)
            Unserialize(is, v[i]);
    }
}

template<typename Stream, typename T, typename A>
inline void Unserialize(Stream& is, std::vector<T, A>& v)
{
    Unserialize_impl(is, v, T());
}
```

### ReadCompactSize的实现
和WriteCompactSize对应
    - 先获取一个字节数据chSize
    - 根据chSize的大小
        - < 253
            + 则chSize就是需要读取的字节数
        - == 253
            + 则再读取双字节数据作为字节数大小
            + 得到的字节数大小不应该< 253
        - == 254
            + 则再读取4字节数据作为字节数大小
            + 得到的字节数大小不应该< 0x10000u (即为unsigned short最大值+1)
        - 否则
            + 则再读取8字节数据作为字节数大小
            + 得到的字节数大小不应该< 0x100000000ULL (即为unsigned int最大值+1)
    - 最终得到的字节大小，不能超过MAX_SIZE
```
template<typename Stream>
uint64_t ReadCompactSize(Stream& is)
{
    uint8_t chSize = ser_readdata8(is);
    uint64_t nSizeRet = 0;
    if (chSize < 253)
    {
        nSizeRet = chSize;
    }
    else if (chSize == 253)
    {
        nSizeRet = ser_readdata16(is);
        if (nSizeRet < 253)
            throw std::ios_base::failure("non-canonical ReadCompactSize()");
    }
    else if (chSize == 254)
    {
        nSizeRet = ser_readdata32(is);
        if (nSizeRet < 0x10000u)
            throw std::ios_base::failure("non-canonical ReadCompactSize()");
    }
    else
    {
        nSizeRet = ser_readdata64(is);
        if (nSizeRet < 0x100000000ULL)
            throw std::ios_base::failure("non-canonical ReadCompactSize()");
    }
    if (nSizeRet > (uint64_t)MAX_SIZE)
        throw std::ios_base::failure("ReadCompactSize(): size too large");
    return nSizeRet;
}
```

### read方法的实现
通过成员变量nReadPos记录读取到的字节位置

- 先计算出本次读取的位置末尾
- 如果超过了整个vch的大小
    + 则抛出异常
- 如果等于整个vch的大小
    + 则复制剩余的所有数据
    + 重置nReadPos位置
    + 清理vch
- 否则
    + 复制对应字节数
    + 移动nReadPos位置到本次读取末尾
```c++
void read(char* pch, size_t nSize)
{
    if (nSize == 0) return;

    // Read from the beginning of the buffer
    unsigned int nReadPosNext = nReadPos + nSize;
    if (nReadPosNext > vch.size())
    {
        throw std::ios_base::failure("CDataStream::read(): end of data");
    }
    memcpy(pch, &vch[nReadPos], nSize);        
    if (nReadPosNext == vch.size())
    {
        nReadPos = 0;
        vch.clear();
        return;
    }
    nReadPos = nReadPosNext;
}
```