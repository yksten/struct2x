#include "BufferReader.h"


BufferReader::BufferReader(BufferInterface& buffer)
    :_buffer(buffer){
}

BufferReader::~BufferReader(){
}

const char* BufferReader::data() const{
    return _buffer.data();
}

size_t BufferReader::size() const{
    return _buffer.size();
}

BufferReader& BufferReader::operator <<(const bool& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int8_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint8_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int16_t& value){
    
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint16_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int32_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint32_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const int64_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const uint64_t& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const float& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const double& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const std::string& value){
    _buffer.setValue(value);
    return *this;
}

BufferReader& BufferReader::operator <<(const char* value)
{
    if (value)
        _buffer.setValue(value);
    else
        _buffer.setValue(0);
    return *this;
}

BufferReader& BufferReader::operator <<(const std::vector<bool>& value)
{
    uint32_t size = value.size();
    this->operator<<(size);
    for (uint32_t i = 0; i < size; ++i){
        bool b = value.at(i);
        this->operator<<(b);
    }
    return *this;
}
