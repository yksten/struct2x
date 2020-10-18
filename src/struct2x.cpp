#include "struct2x.h"

namespace struct2x {

    BufferWrapper::BufferWrapper() : _index(0), _bCalculateFlag(false), _cumtomFieldSize(0) {
    }

    void BufferWrapper::append(const void* data, size_t len) {
        if (_bCalculateFlag) {
            _cumtomFieldSize += len;
        } else {
            _buffer.append((const char*)data, len);
            _index += len;
        }
    }

    void BufferWrapper::startCalculateSize() {
        _bCalculateFlag = true;
        _cumtomFieldSize = 0;
    }

}