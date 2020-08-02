#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <cstdint>
#include <vector>
#include <string>
#include <assert.h>

#include "bufferInterface.h"

class BufferDemo : public BufferInterface {
    std::vector<char> buffer_;
    size_t inIndex_;
    size_t outIndex_;
    static const size_t kInitialSize = 1024;
public:

    explicit BufferDemo(size_t initialSize = kInitialSize)
        : buffer_(initialSize), inIndex_(0), outIndex_(0) {
    }

    const char* data() const { return begin(); }
    size_t size() const { return inIndex_; }

    void setValue(bool value) { append(value); }
    void setValue(int8_t value) { append(value); }
    void setValue(uint8_t value) { append(value); }
    void setValue(int16_t value) { append(value); }
    void setValue(uint16_t value) { append(value); }
    void setValue(int32_t value) { append(value); }
    void setValue(uint32_t value) { append(value); }
    void setValue(int64_t value) { append(value); }
    void setValue(uint64_t value) { append(value); }
    void setValue(float value) { append(value); }
    void setValue(double value) { append(value); }
    void setValue(const std::string& value) { uint32_t size = (uint32_t)value.size(); append(size); append((void*)value.c_str(), size); }
    void setValue(const char* value) { uint32_t size = (uint32_t)strlen(value); append(size); append((void*)value, size); }

    void getValue(bool& value) { value = peekBool(); }
    void getValue(int8_t& value) { value = peek<int8_t>(); }
    void getValue(uint8_t& value) { value = peek<uint8_t>(); }
    void getValue(int16_t& value) { value = peek<int16_t>(); }
    void getValue(uint16_t& value) { value = peek<uint16_t>(); }
    void getValue(int32_t& value) { value = peek<int32_t>(); }
    void getValue(uint32_t& value) { value = peek<uint32_t>(); }
    void getValue(int64_t& value) { value = peek<int64_t>(); }
    void getValue(uint64_t& value) { value = peek<uint64_t>(); }
    void getValue(float& value) { value = getFloat(); }
    void getValue(double& value) { value = getDouble(); }
    void getValue(std::string& value) {
        value.clear();
        uint32_t size = peek<uint32_t>();
        value.append(beginOut(), size);
        hasReader(size);
    }

private:
    template<typename T>
    void append(const T& data) {
        uint32_t nSize = sizeof(T);
        uint8_t szTemp[sizeof(T)] = { 0 };
        for (uint32_t idx = 0; idx < nSize; ++idx) {
            szTemp[idx] = (data >> (idx * 8)) & 0xFF;
        }
        append(szTemp, nSize);
    }
    void append(float v) {
        union { float f; uint32_t i; };
        f = v;
        uint8_t bytes[4] = { 0 };
        bytes[0] = (uint8_t)(i & 0xFF);
        bytes[1] = (uint8_t)((i >> 8) & 0xFF);
        bytes[2] = (uint8_t)((i >> 16) & 0xFF);
        bytes[3] = (uint8_t)((i >> 24) & 0xFF);
        append(bytes, 4);
    }
    void append(double v) {
        union { double db; uint64_t i; };
        db = v;
        uint8_t bytes[8] = { 0 };
        bytes[0] = (uint8_t)(i & 0xFF);
        bytes[1] = (uint8_t)((i >> 8) & 0xFF);
        bytes[2] = (uint8_t)((i >> 16) & 0xFF);
        bytes[3] = (uint8_t)((i >> 24) & 0xFF);
        bytes[4] = (uint8_t)((i >> 32) & 0xFF);
        bytes[5] = (uint8_t)((i >> 40) & 0xFF);
        bytes[6] = (uint8_t)((i >> 48) & 0xFF);
        bytes[7] = (uint8_t)((i >> 56) & 0xFF);
        append(bytes, 8);
    }
    void append(const void* data, size_t len) {
        ensureWritableBytes(len);
        std::copy((const char*)data, (const char*)data + len, beginIn());
        hasWritten(len);
    }
    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len)
            makeSpace(len);
        assert(writableBytes() >= len);
    }
    size_t writableBytes() const { return buffer_.size() - inIndex_; }
    void makeSpace(size_t len) { buffer_.resize(inIndex_ + len); }
    void hasWritten(size_t len) { inIndex_ += len; }
    char* beginIn() { return begin() + inIndex_; }
    char* begin() { return &(*buffer_.begin()); }
    const char* begin()const { return &(*buffer_.begin()); }

private:
    template<typename T>
    T peek() {
        assert(readableBytes() >= sizeof(T));
        uint32_t nSize = sizeof(T);
        uint8_t szTemp[sizeof(T)] = { 0 };
        ::memcpy(&szTemp, beginOut(), nSize);
        T value = T();
        for (uint32_t idx = 0; idx < nSize; ++idx) {
            value |= szTemp[idx] << (idx * 8);
        }
        hasReader(sizeof(T));
        return value;
    }
    bool peekBool() {
        assert(readableBytes() >= sizeof(bool));
        uint32_t nSize = sizeof(bool);
        uint8_t szTemp[sizeof(bool)] = { 0 };
        ::memcpy(&szTemp, beginOut(), nSize);
        hasReader(nSize);
        return (szTemp[0]) ? true : false;
    }
    float getFloat() {
        assert(readableBytes() >= sizeof(float));
        uint8_t szTemp[4] = { 0 };
        ::memcpy(&szTemp, beginOut(), 4);
        union { float f; uint32_t i; };
        i = ((uint32_t)szTemp[0] << 0) | ((uint32_t)szTemp[1] << 8) | ((uint32_t)szTemp[2] << 16) | ((uint32_t)szTemp[3] << 24);
        hasReader(sizeof(float));
        return f;
    }
    double getDouble() {
        assert(readableBytes() >= sizeof(double));
        uint8_t szTemp[8] = { 0 };
        ::memcpy(&szTemp, beginOut(), 8);
        union { double db; uint64_t i; };
        i = ((uint64_t)szTemp[0] << 0) | ((uint64_t)szTemp[1] << 8) | ((uint64_t)szTemp[2] << 16) | ((uint64_t)szTemp[3] << 24) | ((uint64_t)szTemp[4] << 32) | ((uint64_t)szTemp[5] << 40) | ((uint64_t)szTemp[6] << 48) | ((uint64_t)szTemp[7] << 56);
        hasReader(sizeof(double));
        return db;
    }
    size_t readableBytes() const { return buffer_.size() - outIndex_; }
    void hasReader(size_t len) { outIndex_ += len; }
    char* beginOut() { return begin() + outIndex_; }
    //char* begin(){ return &(*buffer_.begin()); }
    //const char* begin()const{ return &(*buffer_.begin()); }
};

#endif