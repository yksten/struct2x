#include "protobuf/decoder.h"
#include <assert.h>
#include "thirdParty/protobuf/proto.h"


namespace proto {


    template <class Dest, class Source>
    inline Dest bit_cast(const Source& source) {
        static_assert(sizeof(Dest) == sizeof(Source), "Sizes do not match");
        Dest dest = Dest();
        memcpy(&dest, &source, sizeof(dest));
        return dest;
    }

    bool ConsumeBytes(const uint8_t*& current, size_t how_many, size_t& remaining) {
        if (how_many > remaining) {
            return false;
        }
        current += how_many;
        remaining -= how_many;
        return true;
    }

    template <class T>
    T ReadFromBytes(const uint8_t*& current, size_t& remaining) {
        bool bConsume = ConsumeBytes(current, sizeof(T), remaining);
        assert(bConsume);
        return *(bit_cast<T*>(current - sizeof(T)));
    }

    void ReadWireTypeAndFieldNumber(const uint8_t*& current, size_t& remaining, uint8_t& wire_type, uint32_t& field_number) {
        const uint8_t wire_type_and_field_number = ReadFromBytes<uint8_t>(current, remaining);
        wire_type = wire_type_and_field_number & 0x07;
        field_number = wire_type_and_field_number >> 3;
        if (field_number >= 16) {
            field_number = field_number & 0xf;
            bool keep_going = false;
            do {
                const uint8_t next_number = ReadFromBytes<uint8_t>(current, remaining);
                keep_going = (next_number >= 128);
                field_number = (field_number << 7) | (next_number & 0x7f);
            } while (keep_going);
        }
    }

    /*=====================================message====================================*/
    uint64_t Message::ReadVarInt(const uint8_t*& current, size_t& remaining) {
        uint64_t result = 0;
        bool keep_going = false;
        int shift = 0;
        do {
            const uint8_t next_number = ReadFromBytes<uint8_t>(current, remaining);
            keep_going = (next_number >= 128);
            result += (uint64_t)(next_number & 0x7f) << shift;
            shift += 7;
        } while (keep_going);
        return result;
    }

    Message::Message(const uint8_t* sz, uint32_t size) :_sz(sz), _size(size) {
    }

    bool Message::ParseFromBytes() {
        const uint8_t* current = _sz;
        size_t remaining = _size;
        while (remaining > 0) {
            uint8_t wire_type = 0;
            uint32_t field_number = 0;
            ReadWireTypeAndFieldNumber(current, remaining, wire_type, field_number);
            switch (wire_type) {
            case WIRETYPE_VARINT: {
                std::map<uint32_t, converter>::const_iterator it = _functionSet.find(field_number);
                if (it != _functionSet.end()) {
                    uint64_t value = ReadVarInt(current, remaining);
                    it->second(&value);
                } else {
                    assert(false);
                }
            } break;
            case WIRETYPE_64BIT: {
                std::map<uint32_t, converter>::const_iterator it = _functionSet.find(field_number);
                if (it != _functionSet.end()) {
                    uint64_t value = ReadFromBytes<uint64_t>(current, remaining);
                    it->second(&value);
                } else {
                    assert(false);
                }
            } break;
            case WIRETYPE_LENGTH_DELIMITED: {
                std::map<uint32_t, converter>::const_iterator it = _functionSet.find(field_number);
                if (it != _functionSet.end()) {
                    const uint64_t size = ReadVarInt(current, remaining);
                    const uint8_t* data = current;
                    current += size;
                    remaining -= size;
                    bin_type bin(data, size);
                    it->second(&bin);
                } else {
                    assert(false);
                }
            } break;
            case WIRETYPE_GROUP_START:
            case WIRETYPE_GROUP_END:
                assert(false);
                return false;
            case WIRETYPE_32BIT: {
                std::map<uint32_t, converter>::const_iterator it = _functionSet.find(field_number);
                if (it != _functionSet.end()) {
                    uint64_t value = ReadFromBytes<uint32_t>(current, remaining);
                    it->second(&value);
                } else {
                    assert(false);
                }
            } break;
            default: {
                return false;
            } break;
            }
        }
        return true;
    }

}  // namespace proto

namespace struct2x {

    PBDecoder::PBDecoder(const uint8_t* sz, unsigned int size)
        : _msg(sz, size) {
    }

    PBDecoder::~PBDecoder() {
    }

    void PBDecoder::decodeValue(serializeItem<bool>& v) {
        _msg.bind<uint64_t, bool>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<int32_t>& v) {
        _msg.bind<uint64_t, int32_t>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<uint32_t>& v) {
        _msg.bind<uint64_t, uint32_t>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<int64_t>& v) {
        _msg.bind<uint64_t, int64_t>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<uint64_t>& v) {
        _msg.bind<uint64_t, uint64_t>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<float>& v) {
        _msg.bind<uint32_t, float>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<double>& v) {
        _msg.bind<uint64_t, double>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeValue(serializeItem<std::string>& v) {
        _msg.bind<proto::bin_type, std::string>(&proto::Message::convertValue, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<bool> >& v) {
        if (v.type == struct2x::TYPE_PACK)
            _msg.bind<proto::bin_type, std::vector<bool> >(&proto::Message::convertArray, v);
        else
            _msg.bind<uint64_t, std::vector<bool> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<int32_t> >& v) {
        if (v.type == struct2x::TYPE_PACK)
            _msg.bind<proto::bin_type, std::vector<int32_t> >(&proto::Message::convertArray, v);
        else
            _msg.bind<uint64_t, std::vector<int32_t> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint32_t> >& v) {
        if (v.type == struct2x::TYPE_PACK)
            _msg.bind<proto::bin_type, std::vector<uint32_t> >(&proto::Message::convertArray, v);
        else
            _msg.bind<uint64_t, std::vector<uint32_t> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<int64_t> >& v) {
        if (v.type == struct2x::TYPE_PACK)
            _msg.bind<proto::bin_type, std::vector<int64_t> >(&proto::Message::convertArray, v);
        else
            _msg.bind<uint64_t, std::vector<int64_t> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint64_t> >& v) {
        if (v.type == struct2x::TYPE_PACK)
            _msg.bind<proto::bin_type, std::vector<uint64_t> >(&proto::Message::convertArray, v);
        else
            _msg.bind<uint64_t, std::vector<uint64_t> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<float> >& v) {
        _msg.bind<uint64_t, std::vector<float> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<double> >& v) {
        _msg.bind<uint64_t, std::vector<double> >(&proto::Message::convertArray, v);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<std::string> >& v) {
        _msg.bind<proto::bin_type, std::vector<std::string> >(&proto::Message::convertArray, v);
    }

    bool PBDecoder::ParseFromBytes() {
        return _msg.ParseFromBytes();
    }

}
