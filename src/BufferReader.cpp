#include "BufferReader.h"
#include <vector>
#include <string>
#include <assert.h>


class BufferIn
{
    std::vector<char> buffer_;
    size_t index_;
    static const size_t kInitialSize = 1024;
public:

    explicit BufferIn(size_t initialSize = kInitialSize)
        : buffer_(initialSize), index_(0){}

    const char* data() const{ return begin(); }
    size_t size() const{ return index_; }

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
    size_t writableBytes() const{ return buffer_.size() - index_; }
    void makeSpace(size_t len){ buffer_.resize(index_ + len); }
    void hasWritten(size_t len){ index_ += len; }
    char* beginIn(){ return begin() + index_; }
    char* begin(){ return &(*buffer_.begin()); }
    const char* begin()const{ return &(*buffer_.begin()); }
};



BufferReader::BufferReader()
    :_buffer(new BufferIn){
    assert(_buffer);
}

BufferReader::~BufferReader(){
    if (_buffer)
        delete _buffer;
}

const char* BufferReader::data() const{
    assert(_buffer);
    return _buffer->data();
}

size_t BufferReader::size() const{
    assert(_buffer);
    return _buffer->size();
}

BufferReader& BufferReader::operator <<(const bool& value){
    assert(_buffer);
    return *this;
}

BufferReader& BufferReader::operator <<(const int8_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint8_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int16_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint16_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int32_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint32_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int64_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint64_t& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const float& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const double& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const std::string& value){
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const char* value)
{
    assert(_buffer);
    _buffer->setValue(value);
    return *this;
}
