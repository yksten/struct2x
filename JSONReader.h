#ifndef __JSON_READER_H__
#define __JSON_READER_H__
#include <string>
#include <vector>
#include <map>


#define SERIALIZATION_1(t, p1) t.addValue(#p1, p1)
#define SERIALIZATION_2(t, p1, p2) SERIALIZATION_1(t, p1).addValue(#p2, p2)
#define SERIALIZATION_3(t, p1, p2, p3) SERIALIZATION_2(t, p1, p2).addValue(#p3, p3)
#define SERIALIZATION_4(t, p1, p2, p3, p4) SERIALIZATION_3(t, p1, p2, p3).addValue(#p4, p4)
#define SERIALIZATION_5(t, p1, p2, p3, p4, p5) SERIALIZATION_4(t, p1, p2, p3, p4).addValue(#p5, p5)
#define SERIALIZATION_6(t, p1, p2, p3, p4, p5, p6) SERIALIZATION_5(t, p1, p2, p3, p4, p5).addValue(#p6, p6)
#define SERIALIZATION_7(t, p1, p2, p3, p4, p5, p6, p7) SERIALIZATION_6(t, p1, p2, p3, p4, p5, p6).addValue(#p7, p7)
#define SERIALIZATION_8(t, p1, p2, p3, p4, p5, p6, p7, p8) SERIALIZATION_7(t, p1, p2, p3, p4, p5, p6, p7).addValue(#p8, p8)
#define SERIALIZATION_9(t, p1, p2, p3, p4, p5, p6, p7, p8, p9) SERIALIZATION_8(t, p1, p2, p3, p4, p5, p6, p7, p8).addValue(#p9, p9)
#define SERIALIZATION_10(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) SERIALIZATION_9(t, p1, p2, p3, p4, p5, p6, p7, p8, p9).addValue(#p10, p10)

#define X_COUNT(TAG, _10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N
#define SERIALIZATION(t, ...)  X_COUNT(SERIALIZATION, __VA_ARGS__, _10,_9,_8,_7,_6,_5,_4,_3,_2,_1) (t, __VA_ARGS__)


struct cJSON;
class JSONReader
{
    cJSON* _root;
    cJSON* _cur;
public:
    JSONReader();
    virtual ~JSONReader();

    template<typename T>
    JSONReader& operator << (const T& value)
    {
        value.serialize(*this);
        return *this;
    }

    JSONReader& addValue(const char* sz, int value);
    JSONReader& addValue(const char* sz, float value);
    JSONReader& addValue(const char* sz, double value);
    JSONReader& addValue(const char* sz, unsigned int value);
    JSONReader& addValue(const char* sz, const char* value);
    JSONReader& addValue(const char* sz, const std::string& value);
    JSONReader& addValue(const char* sz, bool value);
    JSONReader& addValue(const char* sz, const std::vector<int>& value);
    JSONReader& addValue(const char* sz, const std::vector<float>& value);
    JSONReader& addValue(const char* sz, const std::vector<double>& value);
    JSONReader& addValue(const char* sz, const std::vector<std::string>& value);

    template<typename T>
    JSONReader& addValue(const char* sz, const T& value)
    {
        cJSON* curItem = cur();
        createObject(sz);
        value.serialize(*this);
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONReader& addValue(const char* sz, const std::vector<T>& value)
    {
        cJSON* curItem = cur();
        createArray(sz);
        int size = (int)value.size();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            addItemToArray();
            const T& item = value.at(i);
            SERIALIZATION_1((*this), item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONReader& addValue(const char* sz, const std::map<std::string, T>& value)
    {
        cJSON* curItem = cur();
        createArray(sz);
        for (typename std::map<std::string, T>::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            cJSON* lastItem = cur();
            addItemToArray();
            const T& item = it->second;
            addValue(it->first.c_str(), item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
    void toString(std::string& str);
private:
    void createObject(const char* sz);
    void createArray(const char* sz);
    void addItemToArray();
    void cur(cJSON* item) { _cur = item; }
    cJSON* cur() { return _cur; }
};


#endif