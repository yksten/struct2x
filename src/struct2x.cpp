#include "struct2x.h"

namespace struct2x {

    void swap(size_t& first, size_t& second) {
        first = first ^ second;
        second = first ^ second;
        first = first ^ second;
    }

    BufferWrapper::BufferWrapper(size_t nSize) : _buffer(nSize), _index(0) {
    }

    void BufferWrapper::append(const void* data, size_t len) {
        if (_buffer.size() - _index < len)
            _buffer.resize(_index + len);

        std::copy((const char*)data, (const char*)data + len, &(*_buffer.begin()) + _index);

        _index += len;
    }

    void BufferWrapper::swap(BufferWrapper& that) {
        _buffer.swap(that._buffer);
        struct2x::swap(_index, that._index);
    }

}