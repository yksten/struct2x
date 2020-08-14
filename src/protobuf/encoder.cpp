#include "protobuf/encoder.h"

namespace struct2x {
    PBEncoder::writeValue const PBEncoder::functionArray[] = { &PBEncoder::varInt, &PBEncoder::svarInt, &PBEncoder::fixed32, &PBEncoder::fixed64, };
    PBEncoder::PBEncoder(BufferWrapper& buffer) :_buffer(buffer) {
    }

    PBEncoder::~PBEncoder() {
    }

    void PBEncoder::encodeValue(const std::string& v, int32_t type) {
        varInt(v.length());
        _buffer.append(v.c_str(), v.size());
    }

    void PBEncoder::encodeValue(const float& v, int32_t type) {
        union { float f; uint32_t i; };
        f = v;
        uint8_t bytes[4] = { 0 };
        bytes[0] = (uint8_t)(i & 0xFF);
        bytes[1] = (uint8_t)((i >> 8) & 0xFF);
        bytes[2] = (uint8_t)((i >> 16) & 0xFF);
        bytes[3] = (uint8_t)((i >> 24) & 0xFF);
        _buffer.append(bytes, 4);
    }

    void PBEncoder::encodeValue(const double& v, int32_t type) {
        union { double db; uint64_t i; };
        db = v;
        uint8_t bytes[8] = { 0 };
        bytes[0] = (uint8_t)(i & 0xFF);
        bytes[1] = (uint8_t)((i >> 8) & 0xFF);
        bytes[2] = (uint8_t)((i >> 16) & 0xFF);
        bytes[3] = (uint8_t)((i >> 24) & 0xFF);
        bytes[4] = (uint8_t)((i >> 32) & 0xFF);
        bytes[5] = (uint8_t)((i >> 40) & 0xFF);
        bytes[6] = (uint8_t)((i >> 48) & 0xFF);
        bytes[7] = (uint8_t)((i >> 56) & 0xFF);
        _buffer.append(bytes, 8);
    }

    void PBEncoder::value(uint64_t value, int32_t type) {
        (this->*functionArray[type])(value);
    }

    void PBEncoder::varInt(uint64_t value) {
        if (value <= 0x7F) {
            char byte = (char)value;
            _buffer.append(&byte, 1);
        } else {
            encodeVarint32((uint32_t)value, (uint32_t)(value >> 32));
        }
    }

    void PBEncoder::svarInt(uint64_t value) {
        uint64_t zigzagged;
        if (value < 0)
            zigzagged = ~((uint64_t)value << 1);
        else
            zigzagged = (uint64_t)value << 1;
        varInt(zigzagged);
    }

    void PBEncoder::fixed32(uint64_t value) {
        uint32_t val = static_cast<uint32_t>(value);
        uint8_t bytes[4] = { (uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF),
            (uint8_t)((val >> 16) & 0xFF), (uint8_t)((val >> 24) & 0xFF) };
        _buffer.append(bytes, 4);
    }

    void PBEncoder::fixed64(uint64_t value) {
        uint8_t bytes[8] = { (uint8_t)(value & 0xFF), (uint8_t)((value >> 8) & 0xFF),
            (uint8_t)((value >> 16) & 0xFF), (uint8_t)((value >> 24) & 0xFF),
            (uint8_t)((value >> 32) & 0xFF), (uint8_t)((value >> 40) & 0xFF),
            (uint8_t)((value >> 48) & 0xFF), (uint8_t)((value >> 56) & 0xFF) };
        _buffer.append(bytes, 8);
    }

    void PBEncoder::encodeVarint32(uint32_t low, uint32_t high) {
        size_t i = 0;
        char buffer[10] = { 0 };
        char byte = (char)(low & 0x7F);
        low >>= 7;

        while (i < 4 && (low != 0 || high != 0)) {
            byte |= 0x80;
            buffer[i++] = byte;
            byte = (char)(low & 0x7F);
            low >>= 7;
        }

        if (high) {
            byte = (char)(byte | ((high & 0x07) << 4));
            high >>= 3;

            while (high) {
                byte |= 0x80;
                buffer[i++] = byte;
                byte = (char)(high & 0x7F);
                high >>= 7;
            }
        }
        buffer[i++] = byte;
        _buffer.append(buffer, i);
    }
}
