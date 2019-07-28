#include <iostream>
#include <string>
#include <assert.h>

#include "srtuctMacro.h"
#include "JSONReader.h"
#include "JSONWriter.h"

#include "BufferReader.h"
#include "BufferWriter.h"

struct struInfo
{
    struInfo() :no(99){}
    int no;

    bool operator ==(const struInfo& in)const
    {
        if (no != in.no)
            return false;
        return true;
    }

    bool operator !=(const struInfo& in)const 
    {
        return !operator==(in);
    }

    template<typename T>
    void serialize(T& t)
    {
        SERIALIZATION(t, no);
    }
};

struct struItem
{
    int id;
    std::string str;
    struInfo info;
    std::vector<std::string> v;
    std::vector<struInfo> v2;
    std::map<std::string, int> m;
    std::map<std::string, struInfo> m2;

    bool operator ==(const struItem& in)
    {
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

template<typename T>
void serialize(T& t, struItem& item)
{
    NISERIALIZATION(t, item, id, str, info, v, v2, m, m2);
}


void testMap()
{
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

    JSONReader jrmap;
    jrmap << map;
    std::string str2;
    jrmap.toString(str2);

    JSONWriter jwmap(str2.c_str());
    jwmap >> map2;

    assert(map == map2);
}


int main()
{
    testMap();

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

    JSONReader jr;
    jr << item;

    std::string str;
    jr.toString(str);

    JSONWriter jw(str.c_str());
    struItem item2;
    jw >> item2;
    bool b = (item == item2);
    assert(b);


    BufferReader br;
    br << item;

    BufferWriter bw(br.data(), br.size());
    bw >> item2;
    b = (item == item2);
    assert(b);


    return 0;
}