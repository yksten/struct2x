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
    JSONReader(const JSONReader&);
    JSONReader& operator=(const JSONReader&);
public:
    JSONReader();
    virtual ~JSONReader();

    template<typename T>
    JSONReader& convert(const char* sz, const T& value)
    {
        return this->setValue(sz, value);
    }

    template<typename T>
    JSONReader& operator << (const T& value)
    {
        if (T* pValue = const_cast<T*>(&value)){
            serializeWrapper(*this, *pValue);
        }
        return *this;
    }

    template<typename T>
    JSONReader& operator <<(const std::map<std::string, T>& value)
    {
        for (typename std::map<std::string, T>::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            const T& item = it->second;
            setValue(it->first.c_str(), item);
        }
        return *this;
    }

    void toString(std::string& str);
private:
    JSONReader& setValue(const char* sz, int value);
    JSONReader& setValue(const char* sz, float value);
    JSONReader& setValue(const char* sz, double value);
    JSONReader& setValue(const char* sz, unsigned int value);
    JSONReader& setValue(const char* sz, const char* value);
    JSONReader& setValue(const char* sz, const std::string& value);
    JSONReader& setValue(const char* sz, bool value);
    JSONReader& setValue(const char* sz, const std::vector<int>& value);
    JSONReader& setValue(const char* sz, const std::vector<float>& value);
    JSONReader& setValue(const char* sz, const std::vector<double>& value);
    JSONReader& setValue(const char* sz, const std::vector<std::string>& value);

    template<typename T>
    JSONReader& setValue(const char* sz, const T& value)
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
    JSONReader& setValue(const char* sz, const std::vector<T>& value)
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
    JSONReader& setValue(const char* sz, const std::map<std::string, T>& value)
    {
        cJSON* curItem = cur();
        createObject(sz);
        for (typename std::map<std::string, T>::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            cJSON* lastItem = cur();
            const T& item = it->second;
            setValue(it->first.c_str(), item);
            cur(lastItem);
        }
        cur(curItem);
        return *this;
    }
    void createObject(const char* sz);
    void createArray(const char* sz);
    void addItemToArray();
    void cur(cJSON* item) { _cur = item; }
    cJSON* cur() { return _cur; }
};


#endif