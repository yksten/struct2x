#include <assert.h>
#include <iostream>

#include <serialflex/json/decoder.h>
#include <serialflex/json/encoder.h>
#include "exampledata.h"

int main(int argc, char* argv[]) {
    example::containerData ins;
    // 赋值
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

    ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data> >(
        "key", ins.m));
    ins.mm.insert(std::pair<std::string, std::map<int32_t, example::data> >(
        "keystring", ins.m));

    ins.vm.push_back(ins.m);
    ins.vm.push_back(ins.m);

    ins.mv.insert(std::pair<std::string, std::vector<example::data> >(
        "mvkey", ins.v));
    ins.mv.insert(std::pair<std::string, std::vector<example::data> >(
        "mvkeystring", ins.v));

    std::string str_json;
    bool encode_status = serialflex::JSONEncoder(str_json) << ins;
    assert(encode_status);
    std::cout << "json is :" << str_json.c_str() << std::endl;

    example::containerData insResult;
    bool decode_status = serialflex::JSONDecoder(str_json.c_str()) >> insResult;
    assert(decode_status);

    if (ins == insResult) {
        std::cout << "decode success!" << std::endl;
    }

    return 0;
}
