#include <iostream>
#include <assert.h>

#include "cjson/CJSONEncoder.h"
#include "cjson/CJSONDecoder.h"

#include "protobuf/encoder.h"
#include "protobuf/decoder.h"
#include "testStruct.h"

#include "json/encoder.h"
#include "json/decoder.h"


struct struInfo {
    struInfo() :no(99) {}
    int no;
    std::vector<bool> v;
    std::map<int, int> m;

    bool operator ==(const struInfo& in)const {
        if (no != in.no)
            return false;
        if (v != in.v)
            return false;
        return true;
    }

    bool operator !=(const struInfo& in)const {
        return !operator==(in);
    }

    template<typename T>
    void serialize(T& t) {
        SERIALIZATION(t, no, v, m);
    }
};

struct struItem {
    int id;
    std::string str;
    struInfo info;
    std::vector<std::string> v;
    std::vector<struInfo> v2;
    std::map<std::string, int> m;
    std::map<std::string, struInfo> m2;

    bool operator ==(const struItem& in) {
        if (id != in.id)
            return false;
        if (str != in.str)
            return false;
        if (v != in.v)
            return false;
        if (v2 != in.v2)
            return false;
        if (m != in.m)
            return false;
        if (m2 != in.m2)
            return false;
        return true;
    }

    template<typename T>
    void serialize(T& t)
    {
        SERIALIZATION(t, id, str, info, v, v2, m, m2);
    }
};

//VISITSTRUCT(struItem, id, str, info, v, v2, m, m2)
//template<typename T>
//void serialize(T& t, struItem& item)
//{
//    NISERIALIZATION(t, item, id, str, info, v, v2, m, m2);
//}


void testMap() {
    std::map<std::string, int> map, map2;
    map["EVENT_NONE"] = 0;
    map["EVENT_INIT"] = 1;
    map["EVENT_VALUE_OF_NUMBER"] = 2;
    map["EVENT_TIMER_EXPIRED"] = 3;
    map["EVENT_COUNTER_EXPIRED"] = 4;
    map["CONDITION_NONE"] = 0;
    map["CONDITION_NUMBER_COMPARISON"] = 1;
    map["ACTION_NONE"] = 0;
    map["ACTION_DO_NOTHING"] = 1;
    map["ACTION_IF_ELSE"] = 2;
    map["ACTION_IF_ELIF_ELSE"] = 3;
    map["ACTION_ACTION_GROUP"] = 4;
    map["ACTION_SET_NUMBER"] = 5;
    map["ACTION_SET_STRING"] = 6;
    map["ACTION_ADD_NUMBER"] = 7;
    map["ACTION_ADD_STRING"] = 8;
    map["VARIABLE_NONE"] = 0;
    map["VARIABLE_NUMBER"] = 1;
    map["VARIABLE_STRING"] = 2;
    map["VARIABLE_ENTITY"] = 3;
    map["VARIABLE_TIMER"] = 4;
    map["VARIABLE_COUNTER"] = 5;

    struct2x::CJSONEncoder jrmap;
    jrmap << map;
    std::string str2;
    jrmap.toString(str2);

    struct2x::CJSONDecoder jwmap(str2.c_str());
    jwmap >> map2;

    assert(map == map2);
}

void testStructFunc() {
    struItem item;
    item.id = 1;
    item.str = "asdfgh";
    item.info.no = 99;
    item.v.push_back("11");
    item.v.push_back("22");
    struInfo info;
    item.v2.push_back(info);
    info.no = 68;
    item.v2.push_back(info);
    item.m["1"] = 11;
    item.m["2"] = 22;
    item.m2["1"] = info;
    info.no = 992;
    item.m2["2"] = info;

    //struct2x::CJSONEncoder jr;
    //jr << item;
    //std::string str;
    //jr.toString(str);

    struct2x::JSONEncoder encoder;
    encoder << item;
    std::string str;
    encoder.toString(str);

    struct2x::CJSONDecoder jw(str.c_str());
    struItem item2;
    jw >> item2;
    bool b = (item == item2);
    assert(b);
}

struct myStruct {
    std::vector<std::vector<int> > vec;
    template<typename T>
    void serialize(T& t) {
        SERIALIZATION(t, vec);
    }
};

void testVector() {
    myStruct s1, s2;
    std::vector<int> v; v.push_back(1); v.push_back(2); v.push_back(3);
    s1.vec.push_back(v);

    struct2x::CJSONEncoder jr;
    jr << s1;

    std::string str;
    jr.toString(str);

    struct2x::CJSONDecoder jw(str.c_str());
    jw >> s2;
    bool b = (s1.vec == s2.vec);
    assert(b);
}

void testProtobuf() {
    testStruct::struExample item, item2;
    item.id = testStruct::ET2;
    item.str = "example";
    item.f = 9.7f;
    item.db = 19.8f;
    item.v.push_back(1);
    item.v.push_back(2);
    item.v.push_back(3);
    testStruct::struExamples items, items2;
    items.v.push_back(item);
    items.m[1] = item;
    item.id = testStruct::ET3;
    item.str = "afexample";
    item.f = 5.7f;
    item.db = 89.8f;
    items.v.push_back(item);
    items.m[2] = item;

    struct2x::BufferWrapper buffer;
    struct2x::PBEncoder encoder(buffer);
    encoder << items;

    struct2x::CJSONEncoder jr;
    jr << items;
    std::string strJson;
    jr.toString(strJson);
    struct2x::CJSONDecoder(strJson.c_str()) >> items;

    struct2x::PBDecoder decoder(buffer.data(), buffer.size());
    bool b = decoder >> items2;
    assert(b);
}

enum EnumType {
    ET1,
    ET2,
    ET3,
};

struct struExampleEnum {
    struExampleEnum() : id(), has_id(false), has_str(false), f(), db(), e(ET3) {}
    int32_t id;
    bool has_id;
    std::string str;
    bool has_str;
    float f;
    double db;
    std::vector<int32_t> v;
    EnumType e;

    template<typename T>
    void serialize(T& t) {
        SERIALIZATION(t, id, str, f, db, v, e);
        //SERIALIZATION(t, v);
    }
};

int main() {
    //struExampleEnum item;
    //item.e = ET2;
    //struct2x::CJSONDecoder jw("{\"id\":10,\"str\":\"qa\",\"f\":11.0,\"db\":12.0,\"e\":2}");
    //jw >> item;

    //item.v.push_back(15); item.v.push_back(35);
    //std::string mpBuf;
    //struct2x::MPEncoder mpe(mpBuf);
    //mpe << item;

    //for (size_t i = 0; i < mpBuf.size(); ++i)
    //    printf("%02x ", 0xff & mpBuf[i]);
    //printf("\n");

    //struct2x::MPDecoder mpd((const uint8_t*)mpBuf.data(), mpBuf.size());
    //struExampleEnum item2;
    //mpd >> item2;

    struInfo info;
	std::string strJson("{\"no\":\"99\",\"v\":[true, false],\"m\":{\"1\":9, \"2\":29}}");
    struct2x::JSONDecoder decoder(strJson.c_str(), strJson.length());
    decoder >> info;

    struct2x::JSONEncoder encoder;
    encoder << info;
    std::string str;
    encoder.toString(str);

    return 0;

    //testMap();
    //testStructFunc();
    //testVector();
    //testProtobuf();

    return 0;
}