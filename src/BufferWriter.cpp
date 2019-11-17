#include "BufferWriter.h"


BufferWriter::BufferWriter(BufferInterface& buffer)
    :_buffer(buffer){

}

BufferWriter::~BufferWriter(){
}

BufferWriter& BufferWriter::operator >>(bool& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int8_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint8_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int16_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint16_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int32_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint32_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(int64_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(uint64_t& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(float& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(double& value){
    _buffer.getValue(value);
    return *this;
}

BufferWriter& BufferWriter::operator >>(std::string& value){
    _buffer.getValue(value);
    return *this;
}
