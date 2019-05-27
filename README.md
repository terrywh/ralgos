### RALGOS
提供基于 Redis 协议的算法服务，目前包含：
* `address` IPv4 地址映射 `address {ipv4}`，返回字符串数组（城市/详情）（纯真 IP 库）
* `segment` 简单分词, `segment {sentence} [HMM]`, 返回字符串数组（若干词汇），可选 `HMM` 参数启用额外的分词模型（速度有所降低，应对未知词汇时更准确）；

在原公司的使用中发现，基于 Redis 协议提供了相较 HTTP 接口约 4 倍的性能提升；

### 依赖

* https://github.com/yanyiwu/cppjieba

<details><summary>依赖库</summary>
<p>

* boost
```
wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz
tar xf boost_1_70_0.tar.gz
cd boost_1_70_0
./bootstrap.sh --prefix=/data/vendor/boost-1.70.0
./b2 --prefix=/data/vendor/boost-1.70.0 cxxflags="-fPIC" variant=release link=static threading=multi install
```

* hiredis
``` Bash
wget https://github.com/redis/hiredis/archive/v0.14.0.tar.gz -O hiredis-0.14.0.tar.gz
tar xf hiredis-0.14.0.tar.gz
cd hiredis-0.14.0.tar.gz
CC=gcc make
PREFIX=/data/vendor/hiredis-0.14.0 make install
# rm /data/vendor/hiredis-0.14.0/lib/*.so*
```

* iconv
``` Bash
wget https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz
tar xf libiconv-1.16.tar.gz
cd libiconv-1.16
CC=gcc CFLAGS=-fPIC ./configure --enable-static=yes --enable-shared=no --prefix=/data/vendor/iconv-1.16.0
```

* cppjieba
```
wget https://github.com/yanyiwu/cppjieba/archive/866d0e83b017038c90626bdb86ed651681c2904b.zip -O cppjieba-866d0e8.zip
unzip cppjieba-866d0e8.zip
cd cppjieba-866d0e83b017038c90626bdb86ed651681c2904b
mkdir -p /data/vendor/cppjieba-866d0e8/include
cp -r include/cppjieba /data/vendor/cppjieba-866d0e8/include
cp -r deps/limonp /data/vendor/cppjieba-866d0e8/include
# 词库
# cp dict/*.utf8 /path/to/var/
```

* qqwry.dat
```
curl https://github.com/out0fmemory/qqwry.dat/raw/master/qqwry_lastest.dat -o qqwry.dat
```

</p>
</details>
