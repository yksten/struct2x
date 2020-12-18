#include "protobuf/encoder.h"
#include <assert.h>

namespace serialize {

    BufferWrapper::BufferWrapper(std::string* str) : _buffer(str), _target(NULL), _bCalculateFlag(false), _cumtomFieldSize(0) {
    }

    size_t BufferWrapper::size() const {
        size_t temp = (_target - (uint8_t*)&*_buffer->begin());
        return temp;
    }

    std::string* BufferWrapper::buffer() {
        return _buffer;
    }

    uint8_t*& BufferWrapper::target() {
        return _target;
    }

    void BufferWrapper::reserve(uint32_t nSize) {
        assert(_buffer);
        assert(!_target);
        assert(_buffer->capacity() < nSize);
        _buffer->resize(nSize);
        _target = (uint8_t*)&*_buffer->begin();
    }

    void BufferWrapper::appendBytes(const void* data, size_t len) {
        if (!len) return;

        if (_bCalculateFlag) {
            _cumtomFieldSize += len;
        } else {
            for (uint32_t idx = 0; idx < len; ++idx) {
                *_target++ = ((const uint8_t*)data)[idx];
            }
            /*memcpy(_target, data, len);
            _target += len;*/
        }
    }

    void BufferWrapper::appendLength(uint32_t nLength) {
        assert(_bCalculateFlag);
        _cumtomFieldSize += nLength;
    }

    bool BufferWrapper::isGetLength() const {
        return _bCalculateFlag;
    }

    std::pair<bool, uint32_t> BufferWrapper::getCustomField() const {
        std::pair<bool, uint32_t> temp(_bCalculateFlag, _cumtomFieldSize);
        _bCalculateFlag = true;
        _cumtomFieldSize = 0;
        return temp;
    }

    void BufferWrapper::setCustomField(const std::pair<bool, uint32_t>& pair) {
        _bCalculateFlag = pair.first;
        _cumtomFieldSize = pair.second;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////

    PBEncoder::encodeFunction32 PBEncoder::convsetSet32[] = { &PBEncoder::encodeValueVarintArray, &PBEncoder::encodeValueSvarintArray, &PBEncoder::encodeValueFixed32Array };
    PBEncoder::encodeFunction32 PBEncoder::convsetSetPack32[] = { &PBEncoder::encodeValueVarintPack, &PBEncoder::encodeValueSvarintPack, &PBEncoder::encodeValueFixed32Pack };
    PBEncoder::encodeFunction64 PBEncoder::convsetSet64[] = { &PBEncoder::encodeValueVarintArray, &PBEncoder::encodeValueSvarintArray, NULL, &PBEncoder::encodeValueFixed64Array };
    PBEncoder::encodeFunction64 PBEncoder::convsetSetPack64[] = { &PBEncoder::encodeValueVarintPack, &PBEncoder::encodeValueSvarintPack, NULL, &PBEncoder::encodeValueFixed64Pack };

    PBEncoder::PBEncoder(std::string& str) :_buffer(&str), _mgr(NULL) {
    }

    PBEncoder::~PBEncoder() {
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<int32_t> >& value) {
        if (value.type & 0xFFFF) {
            _mgr->bindPack(convsetSetPack32[value.type & 0xFFFF], *(const serializeItem<std::vector<uint32_t> >*)(&value));
        } else {
            _mgr->bindArray(convsetSet32[value.type & 0xFFFF], *(const serializeItem<std::vector<uint32_t> >*)(&value));
        }
        return *this;
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<uint32_t> >& value) {
        if (value.type & 0xFFFF) {
            _mgr->bindPack(convsetSetPack32[value.type & 0xFFFF], value);
        } else {
            _mgr->bindArray(convsetSet32[value.type & 0xFFFF], value);
        }
        return *this;
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<int64_t> >& value) {
        if (value.type & 0xFFFF) {
            _mgr->bindPack(convsetSetPack64[value.type & 0xFFFF], *(const serializeItem<std::vector<uint64_t> >*)(&value));
        } else {
            _mgr->bindArray(convsetSet64[value.type & 0xFFFF], *(const serializeItem<std::vector<uint64_t> >*)(&value));
        }
        return *this;
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<uint64_t> >& value) {
        if (value.type & 0xFFFF) {
            _mgr->bindPack(convsetSetPack64[value.type & 0xFFFF], value);
        } else {
            _mgr->bindArray(convsetSet64[value.type & 0xFFFF], value);
        }
        return *this;
    }

    void PBEncoder::encodeField(const serializeItem<bool>& value) {
        assert(value.type == TYPE_VARINT);
        _mgr->bindValue(&PBEncoder::encodeValue, value);
    }

    void PBEncoder::encodeField(const serializeItem<int32_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, *(const serializeItem<uint32_t>*)(&value));
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint32, *(const serializeItem<uint32_t>*)(&value));
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed32, *(const serializeItem<uint32_t>*)(&value));
        }
    }

    void PBEncoder::encodeField(const serializeItem<uint32_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, value);
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint32, value);
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed32, value);
        }
    }

    void PBEncoder::encodeField(const serializeItem<int64_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, *(const serializeItem<uint64_t>*)(&value));
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint64, *(const serializeItem<uint64_t>*)(&value));
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed64, *(const serializeItem<uint64_t>*)(&value));
        }
    }

    void PBEncoder::encodeField(const serializeItem<uint64_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, value);
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint64, value);
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed64, value);
        }
    }

    void PBEncoder::encodeField(const serializeItem<float>& value) {
        _mgr->bindValue(&PBEncoder::encodeValue, value);
    }

    void PBEncoder::encodeField(const serializeItem<double>& value) {
        _mgr->bindValue(&PBEncoder::encodeValue, value);
    }

    void PBEncoder::encodeField(const serializeItem<std::string>& value) {
        _mgr->bindValue(&PBEncoder::encodeValue, value);
    }

    static inline uint8_t VarintSize(uint32_t value) {
        uint8_t i = 0;;
        while (value >= 0x80) {
            ++i; value >>= 7;
        }
        return ++i;
    }

    void PBEncoder::varInt(uint64_t value, BufferWrapper& buf) {
        if (buf.isGetLength()) {
            buf.appendLength(VarintSize(value));
        } else {
            uint8_t*& target = buf.target();
            while (value >= 0x80) {
                *target++ = static_cast<uint8_t>(value | 0x80);
                value >>= 7;
            }
            *target++ = static_cast<uint8_t>(value);
        }
    }

    void PBEncoder::svarInt(uint32_t value, BufferWrapper& buf) {
        varInt((value << 1) ^ static_cast<uint32_t>(value >> 31), buf);
    }

    void PBEncoder::svarInt(uint64_t value, BufferWrapper& buf) {
        varInt((value << 1) ^ static_cast<uint64_t>(value >> 63), buf);
    }

    void PBEncoder::fixed32(uint32_t value, BufferWrapper& buf) {
        if (buf.isGetLength()) {
            buf.appendLength(4);
        } else {
            uint8_t*& target = buf.target();
            *target++ = (uint8_t)(value & 0xFF);
            *target++ = (uint8_t)((value >> 8) & 0xFF);
            *target++ = (uint8_t)((value >> 16) & 0xFF);
            *target++ = (uint8_t)((value >> 24) & 0xFF);
        }
    }

    void PBEncoder::fixed64(uint64_t value, BufferWrapper& buf) {
        if (buf.isGetLength()) {
            buf.appendLength(8);
        } else {
            uint8_t*& target = buf.target();
            *target++ = (uint8_t)(value & 0xFF);
            *target++ = (uint8_t)((value >> 8) & 0xFF);
            *target++ = (uint8_t)((value >> 16) & 0xFF);
            *target++ = (uint8_t)((value >> 24) & 0xFF);
            *target++ = (uint8_t)((value >> 32) & 0xFF);
            *target++ = (uint8_t)((value >> 40) & 0xFF);
            *target++ = (uint8_t)((value >> 48) & 0xFF);
            *target++ = (uint8_t)((value >> 56) & 0xFF);
        }
    }

    PBEncoder::enclosure_t PBEncoder::encodeVarint(uint64_t tag, uint32_t type, bool* pHas) {
        enclosure_t info(type, 0, pHas);

        while (tag >= 0x80) {
            info.sz[info.size++] = static_cast<uint8_t>(tag | 0x80);
            tag >>= 7;
        }
        info.sz[info.size++] = static_cast<uint8_t>(tag);

        return info;
    }

    void PBEncoder::encodeValue(const bool& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            if (buf.isGetLength()) {
                buf.appendLength(info.size + 1);
            } else {
                buf.appendBytes(info.sz, info.size);
                uint8_t byte = (uint8_t)v;
                buf.appendBytes(&byte, 1);
            }
        }
    }

    void PBEncoder::encodeValue(const int32_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            varInt(v, buf);
        }
    }

    void PBEncoder::encodeValue(const uint32_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            varInt(int32_t(v), buf);
        }
    }

    void PBEncoder::encodeValue(const int64_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            varInt(v, buf);
        }
    }

    void PBEncoder::encodeValue(const uint64_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            varInt(v, buf);
        }
    }

    void PBEncoder::encodeValue(const float& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || value) {
            buf.appendBytes(info.sz, info.size);
            fixed32(*reinterpret_cast<const uint32_t*>(&value), buf);
        }
    }

    void PBEncoder::encodeValue(const double& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || value) {
            buf.appendBytes(info.sz, info.size);
            fixed64(*reinterpret_cast<const uint64_t*>(&value), buf);
        }
    }

    void PBEncoder::encodeValue(const std::string& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || !v.empty()) {
            buf.appendBytes(info.sz, info.size);
            varInt(v.length(), buf);
            buf.appendBytes(v.c_str(), v.length());
        }
    }

    void PBEncoder::encodeValueSvarint32(const uint32_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            svarInt(v, buf);
        }
    }

    void PBEncoder::encodeValueSvarint64(const uint64_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            svarInt(v, buf);
        }
    }

    void PBEncoder::encodeValueFixed32(const uint32_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            fixed32(v, buf);
        }
    }

    void PBEncoder::encodeValueFixed64(const uint64_t& v, const enclosure_t& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.appendBytes(info.sz, info.size);
            fixed64(v, buf);
        }
    }

    void PBEncoder::encodeValueVarintArray(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValue(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarintArray(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueSvarint32(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed32Array(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueFixed32(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueVarintArray(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValue(value.at(i), info, buf);
            }
        }
    }
    
    void PBEncoder::encodeValueSvarintArray(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueSvarint64(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed64Array(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueFixed64(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueVarintPack(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.appendBytes(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                varInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarintPack(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.appendBytes(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                svarInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed32Pack(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.appendBytes(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                fixed32(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueVarintPack(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.appendBytes(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                varInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarintPack(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.appendBytes(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                svarInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed64Pack(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.appendBytes(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                fixed64(value.at(i), buf);
            }
        }
    }

}

