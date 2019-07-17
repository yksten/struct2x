#include <iostream>
#include <string>
#include <assert.h>

#include "srtuctMacro.h"
#include "JSONReader.h"
#include "JSONWriter.h"

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

int main()
{
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

    return 0;
}