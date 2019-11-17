#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <cstdint>
#include <vector>
#include <string>
#include <assert.h>

#include "bufferInterface.h"

class BufferDemo : public BufferInterface
{
    std::vector<char> buffer_;
    size_t inIndex_;
    size_t outIndex_;
    static const size_t kInitialSize = 1024;
public:

    explicit BufferDemo(size_t initialSize = kInitialSize)
        : buffer_(initialSize), inIndex_(0), outIndex_(0){}

    const char* data() const{ return begin(); }
    size_t size() const{ return inIndex_; }

    void setValue(bool value){ append(value); }
    void setValue(int8_t value){ append(value); }
    void setValue(uint8_t value){ append(value); }
    void setValue(int16_t value){ append(value); }
    void setValue(uint16_t value){ append(value); }
    void setValue(int32_t value){ append(value); }
    void setValue(uint32_t value){ append(value); }
    void setValue(int64_t value){ append(value); }
    void setValue(uint64_t value){ append(value); }
    void setValue(float value){ append(value); }
    void setValue(double value){ append(value); }
    void setValue(const std::string& value){ uint32_t size = value.size(); append(size); append(value.c_str(), size); }
    void setValue(const char* value){ uint32_t size = strlen(value); append(size); append(value, size); }

    void getValue(bool& value){ value = peek<bool>(); }
    void getValue(int8_t& value){ value = peek<int8_t>(); }
    void getValue(uint8_t& value){ value = peek<uint8_t>(); }
    void getValue(int16_t& value){ value = peek<int16_t>(); }
    void getValue(uint16_t& value){ value = peek<uint16_t>(); }
    void getValue(int32_t& value){ value = peek<int32_t>(); }
    void getValue(uint32_t& value){ value = peek<uint32_t>(); }
    void getValue(int64_t& value){ value = peek<int64_t>(); }
    void getValue(uint64_t& value){ value = peek<uint64_t>(); }
    void getValue(float& value){ value = peek<float>(); }
    void getValue(double& value){ value = peek<double>(); }
    void getValue(std::string& value){
        value.clear();
        uint32_t size = peek<uint32_t>();
        value.append(beginOut(), size);
        hasReader(size);
    }

private:
    template<typename T>
    void append(const T& data){
        append(static_cast<const char*>((const void*)&data), sizeof(T));
    }
    void append(const char* data, size_t len){
        ensureWritableBytes(len);
        std::copy(data, data + len, beginIn());
        hasWritten(len);
    }
    void ensureWritableBytes(size_t len){
        if (writableBytes() < len)
            makeSpace(len);
        assert(writableBytes() >= len);
    }
    size_t writableBytes() const{ return buffer_.size() - inIndex_; }
    void makeSpace(size_t len){ buffer_.resize(inIndex_ + len); }
    void hasWritten(size_t len){ inIndex_ += len; }
    char* beginIn(){ return begin() + inIndex_; }
    char* begin(){ return &(*buffer_.begin()); }
    const char* begin()const{ return &(*buffer_.begin()); }

private:
    template<typename T>
    T peek(){
        assert(readableBytes() >= sizeof(T));
        T value = T();
        ::memcpy(&value, beginOut(), sizeof(T));
        hasReader(sizeof(T));
        return value;
    }
    size_t readableBytes() const { return buffer_.size() - outIndex_; }
    void hasReader(size_t len){ outIndex_ += len; }
    char* beginOut(){ return begin() + outIndex_; }
    //char* begin(){ return &(*buffer_.begin()); }
    //const char* begin()const{ return &(*buffer_.begin()); }
};

#endif