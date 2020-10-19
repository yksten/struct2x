#include "msgpack/decoder.h"

namespace msgpack {

    Message::Message(const uint8_t* sz, uint32_t size) :_sz(sz), _size(size) {
    }

    uint32_t Message::getFieldSize(const uint8_t*& sz, size_t& size) {
        uint32_t nResult = 0;
        if (0x80 <= *sz && *sz <= 0x8f) { // FixMap
            nResult = (*sz - 0x80);
            ++sz;
            --size;
        } else if (0xde == *sz) { // map 16
            for (int8_t shift = 0; shift < 2; ++shift) {
                const uint8_t next_number = sz[shift + 1];
                nResult += (next_number & 0x7f) << shift;
                nResult += (uint32_t)(next_number & 0x7f) << shift;
            }
            sz += 3;
            size -= 3;
        } else if (0xdf== *sz) { // map 32
            for (int8_t shift = 0; shift < 4; ++shift) {
                const uint8_t next_number = sz[shift + 1];
                nResult += (next_number & 0x7f) << shift;
                nResult += (uint32_t)(next_number & 0x7f) << shift;
            }
            sz += 5;
            size -= 5;
        }
        return nResult;
    }

    bool Message::getFieldName(const uint8_t*& sz, size_t& size, msgpack::bin_type& result) {
        bool bResult = false;
        uint32_t nLength = 0;
        if (0xa0 <= *sz && *sz <= 0xaf) { // FixStr
            nLength = (*sz - 0xa0);
            bResult = true;
            result.first = sz + 1;
            result.second = nLength;
            sz += (1 + nLength);
            size -= (1 + nLength);
        } else if (0xd9 == *sz) { // str 8
            for (int8_t shift = 0; shift < 1; ++shift) {
                const uint8_t next_number = sz[shift + 1];
                nLength += (next_number & 0x7f) << shift;
                nLength += (uint32_t)(next_number & 0x7f) << shift;
            }
            bResult = true;
            result.first = sz + 1;
            result.second = nLength;
            sz += (2 + nLength);
            size -= (2 + nLength);
        } else if (0xda == *sz) { // str 16
            for (int8_t shift = 0; shift < 2; ++shift) {
                const uint8_t next_number = sz[shift + 1];
                nLength += (next_number & 0x7f) << shift;
                nLength += (uint32_t)(next_number & 0x7f) << shift;
            }
            bResult = true;
            result.first = sz + 1;
            result.second = nLength;
            sz += (3 + nLength);
            size -= (3 + nLength);
        } else if (0xdb == *sz) { // str 32
            for (int8_t shift = 0; shift < 4; ++shift) {
                const uint8_t next_number = sz[shift + 1];
                nLength += (next_number & 0x7f) << shift;
                nLength += (uint32_t)(next_number & 0x7f) << shift;
            }
            bResult = true;
            result.first = sz + 1;
            result.second = nLength;
            sz += (5 + nLength);
            size -= (5 + nLength);
        }
        return bResult;
    }

    bool Message::getValue(const uint8_t*& sz, size_t& size, Value& value) {
        bool bResult = false;
        switch (*sz) {
            case 0xc2:{ // false
                value.type = Value::VALUEBOOL;
                value.b = false;
                bResult = true;
                ++sz;
                --size;
            }break;
            case 0xc3: { // true
                value.type = Value::VALUEBOOL;
                value.b = true;
                bResult = true;
                ++sz;
                --size;
            }break;
            case 0xca: {  // float
                ++sz; --size;
                union { float f; uint32_t i; };
                i = 0;
                for (int8_t idx = 0; idx < 4; ++idx) {
                    const uint8_t next_number = sz[idx];
                    i <<= (sizeof(uint8_t) * 8);
                    i += (uint32_t)(next_number & 0x7f);
                }
                sz += 4; size -= 4;
                value.type = Value::VALUEFLOAT;
                value.f = f;
                bResult = true;
            }break;
            case 0xcb: { // double
                ++sz; --size;
                union { double db; uint64_t i; };
                i = 0;
                for (int8_t idx = 0; idx < 8; ++idx) {
                    const uint8_t next_number = sz[idx];
                    i <<= (sizeof(uint8_t) * 8);
                    i += (uint64_t)(next_number & 0x7f);
                }
                sz += 8; size -= 8;
                value.type = Value::VALUEDOUBLE;
                value.db = db;
                bResult = true;
            }break;
            case 0xcc: { // unsigned int  8
                value.type = Value::VALUEINT;
            }break;
            case 0xd0: { // signed int  8
                value.type = Value::VALUEINT;
            }break;
            case 0xcd: { // unsigned int 16
                value.type = Value::VALUEINT;
            }break;
            case 0xd1: { // signed int 16
                value.type = Value::VALUEINT;
            }break;
            case 0xce: { // unsigned int 32
                value.type = Value::VALUEINT;
            }break;
            case 0xd2: { // signed int 32
                value.type = Value::VALUEINT;
            }break;
            case 0xcf: { // unsigned int 64
                value.type = Value::VALUEINT;
            }break;
            case 0xd3: { // signed int 64
                value.type = Value::VALUEINT;
            }break;
                //...TODO
            default: {
                if (0x00 <= *sz && *sz <= 0x7f) { //int
                    value.type = Value::VALUEINT;
                    value.i = *sz - 0x00;
                    bResult = true;
                    ++sz; --size;
                } else {
                    value.type = Value::VALUESTR;
                    bResult = getFieldName(sz, size, value.bin);
                    return bResult;
                }
            }
        }

        return bResult;
    }

    bool Message::ParseFromBytes() {
        const uint8_t* current = _sz;
        size_t remaining = _size;
        uint32_t nFieldSize = getFieldSize(current, remaining);
        while (remaining > 0) {
            // getFieldName
            msgpack::bin_type fieldName;
            if (getFieldName(current, remaining, fieldName)) {
                // getValue
                if (const Message::converter* pFunction = getFunction((const char*)fieldName.first, fieldName.second)) {
                    Value value;
                    if (getValue(current, remaining, value)) {
                        switch (value.type) {
                            case Value::VALUEBOOL: {
                                if (!(*pFunction)(&value.b))
                                    return false;
                            }break;
                            case Value::VALUEINT: {
                                if (!(*pFunction)(&value.i))
                                    return false;
                            }break;
                            case Value::VALUEFLOAT: {
                                if (!(*pFunction)(&value.f))
                                    return false;
                            }break;
                            case Value::VALUEDOUBLE: {
                                if (!(*pFunction)(&value.db))
                                    return false;
                            }break;
                            case Value::VALUESTR: {
                                if (!(*pFunction)(&value.bin))
                                    return false;
                            }break;
                            default:
                                return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    const Message::converter* Message::getFunction(const char* sz, uint32_t size) const {
        for (uint32_t idx = 0; idx < _functionSet.size(); ++idx) {
            const std::pair<const char*, converter>& cur = _functionSet[idx];
            if (!strncmp(sz, cur.first, size)) {
                return &cur.second;
            }
        }
        return NULL;
    }

}

namespace struct2x {
    MPDecoder::MPDecoder(const uint8_t* sz, uint32_t size) :_msg(sz, size) {
    }

    void MPDecoder::decodeValue(const char* sz, bool &v, bool* pHas) {
        _msg.bind<bool, bool>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, int32_t& v, bool* pHas) {
        _msg.bind<int32_t, int32_t>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, uint32_t& v, bool* pHas) {
        _msg.bind<uint32_t, uint32_t>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, int64_t& v, bool* pHas) {
        _msg.bind<int64_t, int64_t>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, uint64_t& v, bool* pHas) {
        _msg.bind<uint64_t, uint64_t>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, float& v, bool* pHas) {
        _msg.bind<float, float>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, double& v, bool* pHas) {
        _msg.bind<double, double>(&MPDecoder::convertValue, sz, v, pHas);
    }

    void MPDecoder::decodeValue(const char* sz, std::string& v, bool* pHas) {
        _msg.bind<msgpack::bin_type, std::string>(&MPDecoder::convertValue, sz, v, pHas);
    }

    bool MPDecoder::ParseFromBytes() {
        return _msg.ParseFromBytes();
    }

}