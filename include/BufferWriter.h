#ifndef __BUFFER_WRITER_H__
#define __BUFFER_WRITER_H__
#include <cstdint>
#include <vector>
#include <map>
#include "SelfAdapt.h"
#include "bufferInterface.h"

class EXPORTAPI BufferWriter {
    BufferInterface& _buffer;
    BufferWriter(const BufferWriter&);
    BufferWriter& operator=(const BufferWriter&);
public:
    BufferWriter(BufferInterface& buffer);
    ~BufferWriter();

    template<typename T>
    BufferWriter& operator >>(T& value) {
        serializeWrapper(*this, value);
        return *this;
    }

    template<typename T>
    BufferWriter& convert(const char* sz, T& value) {
        return this->operator>>(value);
    }

    template<typename T>
    BufferWriter& operator&(T& value) {
        return this->operator>>(value);
    }
private:
    BufferWriter& operator >>(bool& value);
    BufferWriter& operator >>(int8_t& value);
    BufferWriter& operator >>(uint8_t& value);
    BufferWriter& operator >>(int16_t& value);
    BufferWriter& operator >>(uint16_t& value);
    BufferWriter& operator >>(int32_t& value);
    BufferWriter& operator >>(uint32_t& value);
    BufferWriter& operator >>(int64_t& value);
    BufferWriter& operator >>(uint64_t& value);
    BufferWriter& operator >>(float& value);
    BufferWriter& operator >>(double& value);
    BufferWriter& operator >>(std::string& value);

    template<typename T>
    BufferWriter& operator >>(std::vector<T>& value) {
        uint32_t size = 0;
        this->operator>>(size);
        if (size)
            value.clear();
        for (uint32_t i = 0; i < size; ++i) {
            T item;
            this->operator>>(item);
            value.push_back(item);
        }
        return *this;
    }

    template<typename K, typename V>
    BufferWriter& operator >>(std::map<K, V>& value) {
        uint32_t size = 0;
        this->operator>>(size);
        if (size)
            value.clear();
        for (uint32_t i = 0; i < size; ++i) {
            K k; this->operator>>(k);
            V v; this->operator>>(v);
            value.insert(std::pair<K, V>(k, v));
        }
        return *this;
    }
};

#endif