#include "struct2x.h"

namespace struct2x {

    BufferWrapper::BufferWrapper(size_t nSize) : _buffer(nSize), _index(0), _bCalculateFlag(false), _cumtomFieldSize(0) {
    }

    void BufferWrapper::append(const void* data, size_t len) {
        if (_bCalculateFlag) {
            _cumtomFieldSize += len;
        } else {
            if (_buffer.size() - _index < len)
                _buffer.resize(_index + len);

            std::copy((const char*)data, (const char*)data + len, &(*_buffer.begin()) + _index);
            _index += len;
        }
    }

    void BufferWrapper::startCalculateSize() {
        _bCalculateFlag = true;
        _cumtomFieldSize = 0;
    }

}