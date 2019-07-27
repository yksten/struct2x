#ifndef __BUFFER_READER_H__
#define __BUFFER_READER_H__
#include <cstdint>
#include <vector>
#include <map>
#include "srtuctNoInvasiveMacro.h"
#include "SelfAdapt.h"


class BufferIn;
class BufferReader
{
    BufferIn* _buffer;
    BufferReader(const BufferReader&);
    BufferReader& operator=(const BufferReader&);
public:
    BufferReader();
    virtual ~BufferReader();

    template<typename T>
    BufferReader& operator << (const T& value){
        if (T* pValue = const_cast<T*>(&value)){
            serializeWrapper(*this, *pValue);
        }
        return *this;
    }

    template<typename T>
    BufferReader& convert(const char* sz, const T& value){
        return this->operator<<(value);
    }

    template<typename T>
    BufferReader& operator&(const T& value){
        return this->operator<<(value);
    }

    const char* data() const;
    size_t size() const;
private:
    BufferReader& operator <<(const bool& value);
    BufferReader& operator <<(const int8_t& value);
    BufferReader& operator <<(const uint8_t& value);
    BufferReader& operator <<(const int16_t& value);
    BufferReader& operator <<(const uint16_t& value);
    BufferReader& operator <<(const int32_t& value);
    BufferReader& operator <<(const uint32_t& value);
    BufferReader& operator <<(const int64_t& value);
    BufferReader& operator <<(const uint64_t& value);
    BufferReader& operator <<(const float& value);
    BufferReader& operator <<(const double& value);
    BufferReader& operator <<(const std::string& value);
    BufferReader& operator <<(const char* value);

    template<typename T>
    BufferReader& operator <<(const std::vector<T>& value){
        uint32_t size = value.size();
        this->operator<<(size);
        for (uint32_t i = 0; i < size; ++i){
            this->operator<<(value.at(i));
        }
        return *this;
    }

    template<typename K, typename V>
    BufferReader& operator <<(const std::map<K, V>& value){
        uint32_t size = value.size();
        this->operator<<(size);
        for (std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it){
            this->operator<<(it->first);
            this->operator<<(it->second);
        }
        return *this;
    }
};

#endif
