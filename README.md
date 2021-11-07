### struct2x简介
C++98序列化库，一个开源的C++结构体与JSON快速超高效互转库。它可以快速实现结构体对象与JSON对象之间序列化及反序列化要求。快速、简洁的 API 设计，大大降低直接使用JSON解析库来实现此类功能的代码复杂度。也没有外部依赖。  

***注：*** 该库旨在C++结构体和json互转。 

支持结构体类型：
* 支持`bool`、`uint32_t`、`int32_t`、`uint64_t`、`int64_t`、`float`、`double`、`enum`、`std::string`、`std::vector`和`std::map`等。（不支持`std::list`）
* `std::map`的key只能是基础类型，且不能是`float`、`double`。
* 不支持指针和固定数组。（以后也不考虑支持指针和数组）
* 数组内需要统一类型，不支持`[1,2,{"key":"value"}]`。
* 解析时支持has功能。


### 性能(benchmark)
```
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
struct2xJSONEncode      42850 ns        42812 ns        15775
struct2xJSONDecode      13109 ns        13085 ns        56355
cJSONEncode            130396 ns       130237 ns         5382
cJSONDecode             78503 ns        78451 ns         8670
```

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

### 附：benchmark相关代码
```
如果没有大量重复结构，cJSON序列化会更慢
static void struct2xJSONEncode(benchmark::State &state) {
    example::containerData ins;
    // 赋值
    do {
        ins.vi.push_back(1);
        ins.vi.push_back(2345);
        ins.vi.push_back(789);

        ins.v.push_back(ins.d);
        ins.v.push_back(ins.d);

        ins.m.insert(std::pair<int32_t, example::data>(1, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(2, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(3, ins.d));

        ins.vv.push_back(ins.v);
        ins.vv.push_back(ins.v);

        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("key", ins.m));
        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("keystring", ins.m));

        ins.vm.push_back(ins.m);
        ins.vm.push_back(ins.m);

        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkey", ins.v));
        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkeystring", ins.v));
    } while (false);

    while (state.KeepRunning()) {
        std::string strJson;
        bool bEncode = struct2x::JSONEncoder(strJson) << ins;
        assert(bEncode);
    }
}

static void struct2xJSONDecode(benchmark::State &state) {
    example::containerData ins;
    // 赋值
    do {
        ins.vi.push_back(1);
        ins.vi.push_back(2345);
        ins.vi.push_back(789);

        ins.v.push_back(ins.d);
        ins.v.push_back(ins.d);

        ins.m.insert(std::pair<int32_t, example::data>(1, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(2, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(3, ins.d));

        ins.vv.push_back(ins.v);
        ins.vv.push_back(ins.v);

        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("key", ins.m));
        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("keystring", ins.m));

        ins.vm.push_back(ins.m);
        ins.vm.push_back(ins.m);

        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkey", ins.v));
        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkeystring", ins.v));
    } while (false);

    std::string strJson;
    bool bEncode = struct2x::JSONEncoder(strJson) << ins;
    assert(bEncode);
    while (state.KeepRunning()) {
        bool bDecode = struct2x::JSONDecoder(strJson.c_str(), (uint32_t)strJson.size()) >> ins;
        assert(bDecode);
    }
}

static void cJSONEncode(benchmark::State &state) {
    example::containerData ins;
    // 赋值
    do {
        ins.vi.push_back(1);
        ins.vi.push_back(2345);
        ins.vi.push_back(789);

        ins.v.push_back(ins.d);
        ins.v.push_back(ins.d);

        ins.m.insert(std::pair<int32_t, example::data>(1, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(2, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(3, ins.d));

        ins.vv.push_back(ins.v);
        ins.vv.push_back(ins.v);

        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("key", ins.m));
        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("keystring", ins.m));

        ins.vm.push_back(ins.m);
        ins.vm.push_back(ins.m);

        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkey", ins.v));
        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkeystring", ins.v));
    } while (false);

    while (state.KeepRunning()) {
        cJSON *root      = cJSON_CreateObject();
        cJSON *item_d    = cJSON_CreateObject();
        example::data &d = ins.d;
        cJSON_AddNumberToObject(item_d, "d", d.e);
        cJSON_AddNumberToObject(item_d, "i", d.i);
        cJSON_AddNumberToObject(item_d, "ui", d.ui);
        cJSON_AddNumberToObject(item_d, "i64", d.i64);
        cJSON_AddNumberToObject(item_d, "ui64", d.ui64);
        cJSON_AddNumberToObject(item_d, "f", d.f);
        cJSON_AddNumberToObject(item_d, "db", d.db);
        cJSON_AddStringToObject(item_d, "str", d.str.c_str());

        cJSON_AddItemToObject(root, "d", item_d);

        cJSON *item_vi = cJSON_CreateIntArray(&ins.vi[0], 3);
        cJSON_AddItemToObject(root, "vi", item_vi);

        cJSON *item_v = cJSON_CreateArray();
        cJSON_AddItemReferenceToArray(item_v, item_d);
        cJSON_AddItemReferenceToArray(item_v, item_d);
        cJSON_AddItemToObject(root, "v", item_v);

        cJSON *item_m = cJSON_CreateObject();
        cJSON_AddItemReferenceToObject(item_m, "1", item_d);
        cJSON_AddItemReferenceToObject(item_m, "2", item_d);
        cJSON_AddItemReferenceToObject(item_m, "3", item_d);
        cJSON_AddItemToObject(root, "m", item_m);

        cJSON *item_vv = cJSON_CreateArray();
        cJSON_AddItemReferenceToArray(item_vv, item_v);
        cJSON_AddItemReferenceToArray(item_vv, item_v);
        cJSON_AddItemToObject(root, "vv", item_vv);

        cJSON *item_mm = cJSON_CreateObject();
        cJSON_AddItemReferenceToObject(item_mm, "key", item_m);
        cJSON_AddItemReferenceToObject(item_mm, "keystring", item_m);
        cJSON_AddItemToObject(root, "mm", item_mm);

        cJSON *item_vm = cJSON_CreateArray();
        cJSON_AddItemReferenceToArray(item_vm, item_m);
        cJSON_AddItemReferenceToArray(item_vm, item_m);
        cJSON_AddItemToObject(root, "vm", item_vm);

        cJSON *item_mv = cJSON_CreateObject();
        cJSON_AddItemReferenceToObject(item_mv, "mvkey", item_v);
        cJSON_AddItemReferenceToObject(item_mv, "mvkeystring", item_v);

        cJSON_AddItemToObject(root, "mv", item_mv);
        char *buf = cJSON_PrintUnformatted(root);

        std::string strJson(buf);
        free(buf);
        cJSON_Delete(root);
    }
}

static void cJSONDecode(benchmark::State &state) {
    example::containerData ins;
    // 赋值
    do {
        ins.vi.push_back(1);
        ins.vi.push_back(2345);
        ins.vi.push_back(789);

        ins.v.push_back(ins.d);
        ins.v.push_back(ins.d);

        ins.m.insert(std::pair<int32_t, example::data>(1, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(2, ins.d));
        ins.m.insert(std::pair<int32_t, example::data>(3, ins.d));

        ins.vv.push_back(ins.v);
        ins.vv.push_back(ins.v);

        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("key", ins.m));
        ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data>>("keystring", ins.m));

        ins.vm.push_back(ins.m);
        ins.vm.push_back(ins.m);

        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkey", ins.v));
        ins.mv.insert(std::pair<std::string, std::vector<example::data>>("mvkeystring", ins.v));
    } while (false);

    cJSON *root      = cJSON_CreateObject();
    cJSON *item_d    = cJSON_CreateObject();
    example::data &d = ins.d;
    cJSON_AddNumberToObject(item_d, "d", d.e);
    cJSON_AddNumberToObject(item_d, "i", d.i);
    cJSON_AddNumberToObject(item_d, "ui", d.ui);
    cJSON_AddNumberToObject(item_d, "i64", d.i64);
    cJSON_AddNumberToObject(item_d, "ui64", d.ui64);
    cJSON_AddNumberToObject(item_d, "f", d.f);
    cJSON_AddNumberToObject(item_d, "db", d.db);
    cJSON_AddStringToObject(item_d, "str", d.str.c_str());

    cJSON_AddItemToObject(root, "d", item_d);

    cJSON *item_vi = cJSON_CreateIntArray(&ins.vi[0], 3);
    cJSON_AddItemToObject(root, "vi", item_vi);

    cJSON *item_v = cJSON_CreateArray();
    cJSON_AddItemReferenceToArray(item_v, item_d);
    cJSON_AddItemReferenceToArray(item_v, item_d);
    cJSON_AddItemToObject(root, "v", item_v);

    cJSON *item_m = cJSON_CreateObject();
    cJSON_AddItemReferenceToObject(item_m, "1", item_d);
    cJSON_AddItemReferenceToObject(item_m, "2", item_d);
    cJSON_AddItemReferenceToObject(item_m, "3", item_d);
    cJSON_AddItemToObject(root, "m", item_m);

    cJSON *item_vv = cJSON_CreateArray();
    cJSON_AddItemReferenceToArray(item_vv, item_v);
    cJSON_AddItemReferenceToArray(item_vv, item_v);
    cJSON_AddItemToObject(root, "vv", item_vv);

    cJSON *item_mm = cJSON_CreateObject();
    cJSON_AddItemReferenceToObject(item_mm, "key", item_m);
    cJSON_AddItemReferenceToObject(item_mm, "keystring", item_m);
    cJSON_AddItemToObject(root, "mm", item_mm);

    cJSON *item_vm = cJSON_CreateArray();
    cJSON_AddItemReferenceToArray(item_vm, item_m);
    cJSON_AddItemReferenceToArray(item_vm, item_m);
    cJSON_AddItemToObject(root, "vm", item_vm);

    cJSON *item_mv = cJSON_CreateObject();
    cJSON_AddItemReferenceToObject(item_mv, "mvkey", item_v);
    cJSON_AddItemReferenceToObject(item_mv, "mvkeystring", item_v);

    cJSON_AddItemToObject(root, "mv", item_mv);
    char *buf = cJSON_PrintUnformatted(root);

    while (state.KeepRunning()) {
        cJSON *decodeRoot = cJSON_Parse(buf);
        cJSON_Delete(decodeRoot);
    }

    cJSON_Delete(root);
    free(buf);
}

BENCHMARK(struct2xJSONEncode);
BENCHMARK(struct2xJSONDecode);
BENCHMARK(cJSONEncode);
BENCHMARK(cJSONDecode);
```