#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__
#include <string>
#include <vector>
#include <map>


#define DESERIALIZATION_1(t, p1) t.getValue(#p1, p1)
#define DESERIALIZATION_2(t, p1, p2) DESERIALIZATION_1(t, p1).getValue(#p2, p2)
#define DESERIALIZATION_3(t, p1, p2, p3) DESERIALIZATION_2(t, p1, p2).getValue(#p3, p3)
#define DESERIALIZATION_4(t, p1, p2, p3, p4) DESERIALIZATION_3(t, p1, p2, p3).getValue(#p4, p4)
#define DESERIALIZATION_5(t, p1, p2, p3, p4, p5) DESERIALIZATION_4(t, p1, p2, p3, p4).getValue(#p5, p5)
#define DESERIALIZATION_6(t, p1, p2, p3, p4, p5, p6) DESERIALIZATION_5(t, p1, p2, p3, p4, p5).getValue(#p6, p6)
#define DESERIALIZATION_7(t, p1, p2, p3, p4, p5, p6, p7) DESERIALIZATION_6(t, p1, p2, p3, p4, p5, p6).getValue(#p7, p7)
#define DESERIALIZATION_8(t, p1, p2, p3, p4, p5, p6, p7, p8) DESERIALIZATION_7(t, p1, p2, p3, p4, p5, p6, p7).getValue(#p8, p8)
#define DESERIALIZATION_9(t, p1, p2, p3, p4, p5, p6, p7, p8, p9) DESERIALIZATION_8(t, p1, p2, p3, p4, p5, p6, p7, p8).getValue(#p9, p9)
#define DESERIALIZATION_10(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) DESERIALIZATION_9(t, p1, p2, p3, p4, p5, p6, p7, p8, p9).getValue(#p10, p10)


struct cJSON;
class JSONWriter
{
    cJSON* _root;
    cJSON* _cur;
public:
    JSONWriter(const char* sz);
    virtual ~JSONWriter();

    template<typename T>
    JSONWriter& operator >> (T& value)
    {
        value.deserialize(*this);
        return *this;
    }

    JSONWriter& getValue(const char* sz, int& value);
    JSONWriter& getValue(const char* sz, float& value);
    JSONWriter& getValue(const char* sz, double& value);
    JSONWriter& getValue(const char* sz, unsigned int& value);
    JSONWriter& getValue(const char* sz, std::string& value);
    JSONWriter& getValue(const char* sz, bool& value);
    JSONWriter& getValue(const char* sz, std::vector<int>& value);
    JSONWriter& getValue(const char* sz, std::vector<float>& value);
    JSONWriter& getValue(const char* sz, std::vector<double>& value);
    JSONWriter& getValue(const char* sz, std::vector<std::string>& value);

    template<typename T>
    JSONWriter& getValue(const char* sz, T& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        value.deserialize(*this);
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONWriter& getValue(const char* sz, std::vector<T>& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        int size = getArraySize();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            getArrayItem(i);
            T item;
            DESERIALIZATION_1((*this), item);
            value.push_back(item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
private:
    void getObject(const char* sz);
    int getArraySize()const;
    void getArrayItem(int i);
    void cur(cJSON* item) { _cur = item; }
    cJSON* cur() { return _cur; }
};


#endif