#ifndef __JSON_READER_H__
#define __JSON_READER_H__
#include <string>
#include <vector>
#include <map>
#include "srtuctNoInvasiveMacro.h"
#include "SelfAdapt.h"


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
        if (T* pValue = const_cast<T*>(&value)){
            serializeWrapper(*this, *pValue);
        }
        return *this;
    }

    JSONReader& convert(const char* sz, int value);
    JSONReader& convert(const char* sz, float value);
    JSONReader& convert(const char* sz, double value);
    JSONReader& convert(const char* sz, unsigned int value);
    JSONReader& convert(const char* sz, const char* value);
    JSONReader& convert(const char* sz, const std::string& value);
    JSONReader& convert(const char* sz, bool value);
    JSONReader& convert(const char* sz, const std::vector<int>& value);
    JSONReader& convert(const char* sz, const std::vector<float>& value);
    JSONReader& convert(const char* sz, const std::vector<double>& value);
    JSONReader& convert(const char* sz, const std::vector<std::string>& value);

    template<typename T>
    JSONReader& convert(const char* sz, const T& value)
    {
        cJSON* curItem = cur();
        createObject(sz);
        if (T* pValue = const_cast<T*>(&value)){
            serializeWrapper(*this, *pValue);
        }
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONReader& convert(const char* sz, const std::vector<T>& value)
    {
        cJSON* curItem = cur();
        createArray(sz);
        int size = (int)value.size();
        for (int i = 0; i < size; ++i)
        {
            cJSON* lastItem = cur();
            addItemToArray();
            const T& item = value.at(i);
            this->operator<<(item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
    template<typename T>
    JSONReader& convert(const char* sz, const std::map<std::string, T>& value)
    {
        cJSON* curItem = cur();
        createArray(sz);
        for (typename std::map<std::string, T>::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            cJSON* lastItem = cur();
            addItemToArray();
            const T& item = it->second;
            convert(it->first.c_str(), item);
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