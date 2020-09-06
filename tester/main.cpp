#include <iostream>
#include <assert.h>

#include "json/JSONEncoder.h"
#include "json/JSONDecoder.h"

#include "protobuf/encoder.h"
#include "protobuf/decoder.h"
#include "testStruct.h"

#include "rapidjson/rapidjsonEncoder.h"
#include "rapidjson/rapidjsonDecoder.h"

struct struInfo {
    struInfo() :no(99) {}
    int no;
    std::vector<bool> v;

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
        SERIALIZATION(t, no, v);
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

    struct2x::JSONEncoder jrmap;
    jrmap << map;
    std::string str2;
    jrmap.toString(str2);

    struct2x::JSONDecoder jwmap(str2.c_str());
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

    //struct2x::JSONEncoder jr;
    //jr << item;
    //std::string str;
    //jr.toString(str);

    struct2x::rapidjsonEncoder encoder;
    encoder << item;
    std::string str;
    encoder.toString(str);

    struct2x::JSONDecoder jw(str.c_str());
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

    struct2x::JSONEncoder jr;
    jr << s1;

    std::string str;
    jr.toString(str);

    struct2x::JSONDecoder jw(str.c_str());
    jw >> s2;
    bool b = (s1.vec == s2.vec);
    assert(b);
}

void testProtobuf() {
    testStruct::struExample item, item2;
    item.id = 1;
    item.str = "example";
    item.f = 9.7f;
    item.db = 19.8f;
    item.v.push_back(1);
    item.v.push_back(2);
    item.v.push_back(3);
    testStruct::struExamples items, items2;
    items.v.push_back(item);
    items.m[1] = item;
    item.id = 2;
    item.str = "afexample";
    item.f = 5.7f;
    item.db = 89.8f;
    items.v.push_back(item);
    items.m[2] = item;

    struct2x::BufferWrapper buffer;
    struct2x::PBEncoder encoder(buffer);
    encoder << items;

    struct2x::JSONEncoder jr;
    jr << items;
    std::string strJson;
    jr.toString(strJson);
    struct2x::JSONDecoder(strJson.c_str()) >> items;

    struct2x::PBDecoder decoder(buffer.data(), buffer.size());
    bool b = decoder >> items2;
    assert(b);
}


int main() {
    //bool b = true;
    //struct2x::converter f = struct2x::bind(&struct2x::rapidjsonDecoder::convertBool, b, NULL);
    //f(&b);

    //struInfo info;
    //struct2x::rapidjsonDecoder decoder("{\"no\":99,\"v\":[true]}");
    //decoder >> info;

    //testMap();
    //testStructFunc();
    //testVector();
    testProtobuf();

    return 0;
}