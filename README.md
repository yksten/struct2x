# <center>struct2x简介</center>
### C++98序列化库
一个开源的C++结构体与JSON快速超高效互转库。它可以快速实现结构体对象与JSON对象之间序列化及反序列化要求。快速、简洁的 API 设计，大大降低直接使用JSON解析库来实现此类功能的代码复杂度。也没有外部依赖。  

***注：*** 该库旨在C++结构体和json互转。 

支持结构体类型：
* 支持`uint32_t`、`int32_t`、`uint64_t`、`int64_t`、`float`、`double`、`enum`、`std::string`、`std::vector`和`std::map`等。（不支持`std::list`）
* `std::map`的key只能是基础类型，且不能是`float`、`double`。
* 不支持指针和固定数组。（以后也不考虑支持指针和数组）
* 数组内需要统一类型，不支持`[1,2,{"key":"value"}]`。
* 解析时支持has功能。

### 如何使用
```
#include <iostream>
#include <struct2x/struct2x.h>      // SERIALIZE
#include <struct2x/json/encoder.h>  // encode（struct -> json）
#include <struct2x/json/decoder.h>  // decode（json -> struct）

struct structTest {
    int32_t id;
    std::string str;
    std::vector<std::string> v;
    std::map<std::string, int> m;
    // 使用该模板函数，指定struct中需要转为json的变量，必须为public
    template<typename T>
    void serialize(T& t) {
        SERIALIZE(t, id, str, v, m);
        // 也可以使用第二种方式
        // t.convert("id", id).convert("str", str).convert("v", v).convert("m", m);
    }
};

int main(int argc, char* argv[]) {
    std::string strJson;
    structTest ins;
    // ··· ins赋值操作
    bool bEncode = serialize::JSONEncoder(strJson) << ins;

    structTest insDecode;
    bool bDecode = serialize::JSONDecoder(strJson.c_str(), strJson.size()) >> insDecode;

    return 0;
}
```
### 编译
```
$ git clone https://github.com/yksten/struct2x.git
$ cd struct2x
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DEXAMPLE=ON ../..
$ make -j
$ popd
```
