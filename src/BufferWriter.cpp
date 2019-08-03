#include "BufferWriter.h"
#include <cstdint>
#include <vector>
#include <string>
#include <assert.h>


class BufferOut
{
    std::vector<char> buffer_;
    size_t index_;
public:

    explicit BufferOut(const char* data, size_t size) :buffer_(data, data + size), index_(0){}

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
    T peek(){
        assert(readableBytes() >= sizeof(T));
        T value = T();
        ::memcpy(&value, beginOut(), sizeof(T));
        hasReader(sizeof(T));
        return value;
    }
    size_t readableBytes() const { return buffer_.size() - index_; }
    void hasReader(size_t len){ index_ += len; }
    char* beginOut(){ return begin() + index_; }
    char* begin(){ return &(*buffer_.begin()); }
    const char* begin()const{ return &(*buffer_.begin()); }
};


BufferWriter::BufferWriter(const char* data, size_t size)
    :_buffer(new BufferOut(data, size)){

}

BufferWriter::~BufferWriter(){
    assert(_buffer);
    if (_buffer)
        delete _buffer;
}

BufferWriter& BufferWriter::operator >>(bool& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int8_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint8_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int16_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint16_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int32_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint32_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int64_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint64_t& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(float& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(double& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(std::string& value){
    assert(_buffer);
    _buffer->getValue(value);
    return *this;
}
