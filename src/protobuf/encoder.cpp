#include "protobuf/encoder.h"
#include <assert.h>

namespace serialize {

    BufferWrapper::BufferWrapper(std::string* str) : _buffer(str), _bCalculateFlag(false), _cumtomFieldSize(0) {
    }

    const uint8_t* BufferWrapper::data() const {
        return (const uint8_t*)_buffer->c_str();
    }

    size_t BufferWrapper::size() const {
        return _buffer->size();
    }

    std::string* BufferWrapper::buffer() {
        return _buffer;
    }

    void BufferWrapper::append(const void* data, size_t len) {
        if (!len) return;

        if (_bCalculateFlag) {
            _cumtomFieldSize += len;
        } else {
            _buffer->append((const char*)data, len);
        }
    }

    bool BufferWrapper::isGetLength() const {
        return _bCalculateFlag;
    }

    void BufferWrapper::append(uint32_t nLength) {
        assert(_bCalculateFlag);
        _cumtomFieldSize += nLength;
    }

    void BufferWrapper::startCalculateSize() {
        _bCalculateFlag = true;
        _cumtomFieldSize = 0;
    }

    std::pair<bool, size_t> BufferWrapper::getCustomField() const {
        return std::pair<bool, size_t>(_bCalculateFlag, _cumtomFieldSize);
    }

    void BufferWrapper::setCustomField(const std::pair<bool, size_t>& pair) {
        _bCalculateFlag = pair.first;
        _cumtomFieldSize = pair.second;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////

    PBEncoder::encodeFunction32 PBEncoder::convsetSet32[] = { &PBEncoder::encodeValueVarint, &PBEncoder::encodeValueSvarint, &PBEncoder::encodeValueFixed32 };
    PBEncoder::encodeFunction32 PBEncoder::convsetSetPack32[] = { &PBEncoder::encodeValueVarintPack, &PBEncoder::encodeValueSvarintPack, &PBEncoder::encodeValueFixed32Pack };
    PBEncoder::encodeFunction64 PBEncoder::convsetSet64[] = { &PBEncoder::encodeValueVarint, &PBEncoder::encodeValueSvarint, NULL, &PBEncoder::encodeValueFixed64 };
    PBEncoder::encodeFunction64 PBEncoder::convsetSetPack64[] = { &PBEncoder::encodeValueVarintPack, &PBEncoder::encodeValueSvarintPack, NULL, &PBEncoder::encodeValueFixed64Pack };

    PBEncoder::PBEncoder(std::string& str) :_buffer(&str) {
    }

    PBEncoder::~PBEncoder() {
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<int32_t> >& value) {
        if (value.type >> 16) {
            _mgr->bindPack(convsetSetPack32[value.type & 0xFFFF], *(const serializeItem<std::vector<uint32_t> >*)(&value));
        } else {
            _mgr->bindArray(convsetSet32[value.type & 0xFFFF], *(const serializeItem<std::vector<uint32_t> >*)(&value));
        }
        return *this;
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<uint32_t> >& value) {
        if (value.type >> 16) {
            _mgr->bindPack(convsetSetPack32[value.type & 0xFFFF], value);
        } else {
            _mgr->bindArray(convsetSet32[value.type & 0xFFFF], value);
        }
        return *this;
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<int64_t> >& value) {
        if (value.type >> 16) {
            _mgr->bindPack(convsetSetPack64[value.type & 0xFFFF], *(const serializeItem<std::vector<uint64_t> >*)(&value));
        } else {
            _mgr->bindArray(convsetSet64[value.type & 0xFFFF], *(const serializeItem<std::vector<uint64_t> >*)(&value));
        }
        return *this;
    }

    PBEncoder& PBEncoder::operator&(const serializeItem<std::vector<uint64_t> >& value) {
        if (value.type >> 16) {
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
            _mgr->bindValue(&PBEncoder::encodeValueSvarint, *(const serializeItem<uint32_t>*)(&value));
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed32, *(const serializeItem<uint32_t>*)(&value));
        }
    }

    void PBEncoder::encodeField(const serializeItem<uint32_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, value);
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint, value);
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed32, value);
        }
    }

    void PBEncoder::encodeField(const serializeItem<int64_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, *(const serializeItem<uint64_t>*)(&value));
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint, *(const serializeItem<uint64_t>*)(&value));
        } else if (value.type == TYPE_FIXED32) {
            _mgr->bindValue(&PBEncoder::encodeValueFixed64, *(const serializeItem<uint64_t>*)(&value));
        }
    }

    void PBEncoder::encodeField(const serializeItem<uint64_t>& value) {
        if (value.type == TYPE_VARINT) {
            _mgr->bindValue(&PBEncoder::encodeValue, value);
        } else if (value.type == TYPE_SVARINT) {
            _mgr->bindValue(&PBEncoder::encodeValueSvarint, value);
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
            if (const uint32_t topbits = static_cast<uint32_t>(value >> 32)) {
                // Top bits are zero, so scan in bottom bits
                buf.append(32 + VarintSize(topbits));
            } else {
                buf.append(VarintSize(static_cast<uint32_t>(value)));
            }
            /*uint8_t i = 0;;
            while (value >= 0x80) {
                ++i; value >>= 7;
            }
            buf.append(++i);*/
        } else {
            uint8_t i = 0;
            uint8_t buffer[10];
            while (value >= 0x80) {
                buffer[i++] = static_cast<uint8_t>(value | 0x80);
                value >>= 7;
            }
            buffer[i++] = static_cast<uint8_t>(value);
            buf.append(buffer, i);
        }
    }

    void PBEncoder::svarInt(uint64_t value, BufferWrapper& buf) {
        uint64_t zigzagged;
        if (value < 0)
            zigzagged = ~((uint64_t)value << 1);
        else
            zigzagged = (uint64_t)value << 1;
        varInt(zigzagged, buf);
    }

    void PBEncoder::fixed32(uint32_t value, BufferWrapper& buf) {
        if (buf.isGetLength()) {
            buf.append(4);
        } else {
            uint8_t bytes[4] = { (uint8_t)(value & 0xFF), (uint8_t)((value >> 8) & 0xFF),
                (uint8_t)((value >> 16) & 0xFF), (uint8_t)((value >> 24) & 0xFF) };
            buf.append(bytes, 4);
        }
    }

    void PBEncoder::fixed64(uint64_t value, BufferWrapper& buf) {
        if (buf.isGetLength()) {
            buf.append(8);
        } else {
            uint8_t bytes[8] = { (uint8_t)(value & 0xFF), (uint8_t)((value >> 8) & 0xFF),
                (uint8_t)((value >> 16) & 0xFF), (uint8_t)((value >> 24) & 0xFF),
                (uint8_t)((value >> 32) & 0xFF), (uint8_t)((value >> 40) & 0xFF),
                (uint8_t)((value >> 48) & 0xFF), (uint8_t)((value >> 56) & 0xFF) };
            buf.append(bytes, 8);
        }
    }

    PBEncoder::enclosure_type PBEncoder::encodeVarint(uint64_t tag, bool* pHas) {
        enclosure_type info(0, pHas);

        while (tag >= 0x80) {
            info.sz[info.size++] = static_cast<uint8_t>(tag | 0x80);
            tag >>= 7;
        }
        info.sz[info.size++] = static_cast<uint8_t>(tag);

        return info;
    }

    void PBEncoder::encodeValue(const bool& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            if (buf.isGetLength()) {
                buf.append(info.size + 1);
            } else {
                buf.append(info.sz, info.size);
                uint8_t byte = (uint8_t)v;
                buf.append(&byte, 1);
            }
        }
    }

    void PBEncoder::encodeValue(const int32_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            varInt(v, buf);
        }
    }

    void PBEncoder::encodeValue(const uint32_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            varInt(int32_t(v), buf);
        }
    }

    void PBEncoder::encodeValue(const int64_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            varInt(v, buf);
        }
    }

    void PBEncoder::encodeValue(const uint64_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            varInt(v, buf);
        }
    }

    void PBEncoder::encodeValue(const float& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || value) {
            buf.append(info.sz, info.size);
            fixed32(*reinterpret_cast<const uint32_t*>(&value), buf);
        }
    }

    void PBEncoder::encodeValue(const double& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || value) {
            buf.append(info.sz, info.size);
            fixed64(*reinterpret_cast<const uint64_t*>(&value), buf);
        }
    }

    void PBEncoder::encodeValue(const std::string& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || !v.empty()) {
            buf.append(info.sz, info.size);
            varInt(v.length(), buf);
            buf.append(v.c_str(), v.length());
        }
    }

    void PBEncoder::encodeValueSvarint(const uint32_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            svarInt(v, buf);
        }
    }

    void PBEncoder::encodeValueFixed32(const uint32_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            fixed32(v, buf);
        }
    }

    void PBEncoder::encodeValueSvarint(const uint64_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            svarInt(v, buf);
        }
    }

    void PBEncoder::encodeValueFixed64(const uint64_t& v, const enclosure_type& info, BufferWrapper& buf) {
        if (!info.pHas || v) {
            buf.append(info.sz, info.size);
            fixed64(v, buf);
        }
    }

    void PBEncoder::encodeValueVarint(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValue(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarint(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueSvarint(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed32(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueFixed32(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueVarint(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValue(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarint(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueSvarint(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed64(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                encodeValueFixed64(value.at(i), info, buf);
            }
        }
    }

    void PBEncoder::encodeValueVarintPack(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.append(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                varInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarintPack(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.append(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                svarInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed32Pack(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.append(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                fixed32(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueVarintPack(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.append(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                varInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueSvarintPack(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.append(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                svarInt(value.at(i), buf);
            }
        }
    }

    void PBEncoder::encodeValueFixed64Pack(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf) {
        if (!value.empty()) {
            buf.append(info.sz, info.size);
            uint32_t size = (uint32_t)value.size();
            for (uint32_t i = 0; i < size; ++i) {
                fixed64(value.at(i), buf);
            }
        }
    }

}

