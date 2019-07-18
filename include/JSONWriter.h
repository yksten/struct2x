#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__
#include <string>
#include <vector>
#include <map>
#include "srtuctNoInvasiveMacro.h"
#include "SelfAdapt.h"


struct cJSON;
class JSONWriter
{
    cJSON* _root;
    cJSON* _cur;
    JSONWriter();
    JSONWriter(const JSONWriter&);
    JSONWriter& operator=(const JSONWriter&);
public:
    JSONWriter(const char* sz);
    virtual ~JSONWriter();

    template<typename T>
    JSONWriter& operator >> (T& value)
    {
        serializeWrapper(*this, value);
        return *this;
    }

    JSONWriter& convert(const char* sz, int& value);
    JSONWriter& convert(const char* sz, float& value);
    JSONWriter& convert(const char* sz, double& value);
    JSONWriter& convert(const char* sz, unsigned int& value);
    JSONWriter& convert(const char* sz, std::string& value);
    JSONWriter& convert(const char* sz, bool& value);
    JSONWriter& convert(const char* sz, std::vector<int>& value);
    JSONWriter& convert(const char* sz, std::vector<float>& value);
    JSONWriter& convert(const char* sz, std::vector<double>& value);
    JSONWriter& convert(const char* sz, std::vector<std::string>& value);

    template<typename T>
    JSONWriter& convert(const char* sz, T& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        serializeWrapper(*this, value);
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONWriter& convert(const char* sz, std::vector<T>& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        int size = getArraySize();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            getArrayItem(i);
            T item;
            this->operator>>(item);
            value.push_back(item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONWriter& convert(const char* sz, std::map<std::string, T>& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        int size = getArraySize();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            getArrayItem(i);
            std::string key;
            T item;
            keyValue(key, item);
            value.insert(std::pair<std::string, T>(key, item));
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
private:
    void getObject(const char* sz);
    int getArraySize()const;
    void getArrayItem(int i);
    const char* getItemName()const;
    template<typename T>
    void keyValue(std::string& key, T& value)
    {
        key = getItemName();
        convert(key.c_str(), value);
    }
    void cur(cJSON* item) { _cur = item; }
    cJSON* cur() { return _cur; }
};


#endif