#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__
#include <string>
#include <vector>
#include <map>
#include "structNoInvasiveMacro.h"
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
    JSONWriter& convert(const char* sz, T& value)
    {
        return this->getValue(sz, value);
    }

    template<typename T>
    JSONWriter& operator >> (T& value)
    {
        serializeWrapper(*this, value);
        return *this;
    }

    template<typename T>
    JSONWriter& operator >>(std::vector<T>& value)
    {
        int size = getArraySize();
        if (size)
            value.clear();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            getArrayItem(i);
            T item;
            this->operator>>(item);
            value.push_back(item);
            cur(lastItem);
        }
        return *this;
    }

    template<typename T>
    JSONWriter& operator >>(std::map<std::string, T>& value)
    {
        int size = getMapSize();
        if (size)
            value.clear();
        for (int i = 0; i < size; ++i)
        {
            std::string key;
            T item;
            getkeyValue(i, key, item);
            value.insert(std::pair<std::string, T>(key, item));
        }
        return *this;
    }
private:
    JSONWriter& operator >>(std::vector<int>& value);
    JSONWriter& operator >>(std::vector<float>& value);
    JSONWriter& operator >>(std::vector<double>& value);
    JSONWriter& operator >>(std::vector<std::string>& value);
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
        serializeWrapper(*this, value);
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONWriter& getValue(const char* sz, std::vector<T>& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        int size = getArraySize();
        if (size)
            value.clear();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            getArrayItem(i);
            typename TypeTraits<T>::Type item;
            this->operator>>(item);
            value.push_back(item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONWriter& getValue(const char* sz, std::map<std::string, T>& value)
    {
        cJSON* curItem = cur();
        getObject(sz);
        int size = getMapSize();
        if (size)
            value.clear();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            std::string key;
            T item;
            getkeyValue(i, key, item);
            value.insert(std::pair<std::string, T>(key, item));
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }

    void getObject(const char* sz);
    int getArraySize()const;
    void getArrayItem(int i);
    int getMapSize() const;
    const char* getChildName(int i)const;
    template<typename T>
    void getkeyValue(int i, std::string& key, T& value)
    {
        key = getChildName(i);
        getValue(key.c_str(), value);
    }
    void cur(cJSON* item) { _cur = item; }
    cJSON* cur() { return _cur; }
};


#endif