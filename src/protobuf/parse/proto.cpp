#include "proto.h"
#include <string>
#include <iostream>
#include <assert.h>


namespace proto {

/*====================================field=====================================*/
    Field::Field(uint32_t num, Field::FieldType t)
        : number(num), type(t) {
    }
    
    Field::~Field() {
        if (type == FIELD_BYTES) {
            if (!_msgs.empty()) {
                for (uint32_t idx = 0; idx < _msgs.size(); ++idx) {
                    delete _msgs.at(idx);
                }
                _msgs.clear();
            }
        }
    }
/*====================================field=====================================*/

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
    Message::Message() {
    }
    
    bool Message::ParseFromBytes(const uint8_t* szBin, size_t nBinSize) {
        const uint8_t* current = szBin;
        size_t remaining = nBinSize;
        while (remaining > 0) {
            uint8_t wire_type = 0;
            uint32_t field_number = 0;
            ReadWireTypeAndFieldNumber(current, remaining, wire_type, field_number);
            switch (wire_type) {
                case WIRETYPE_VARINT: {
                    Field& field = AddField(field_number, Field::FIELD_UINT64);
                    field._uint64s.push_back(ReadVarInt(current, remaining));
                } break;
                case WIRETYPE_64BIT: {
                    Field& field = AddField(field_number, Field::FIELD_UINT64);
                    field._uint64s.push_back(ReadFromBytes<uint64_t>(current, remaining));
                } break;
                case WIRETYPE_LENGTH_DELIMITED: {
                    Field& field = AddField(field_number, Field::FIELD_BYTES);
                    const uint64_t size = ReadVarInt(current, remaining);
                    const uint8_t* data = current;
                    field._bins.push_back(binType(data, size));
                    current += size;
                    remaining -= size;
                } break;
                case WIRETYPE_GROUP_START:
                case WIRETYPE_GROUP_END:
                    return false;
                case WIRETYPE_32BIT: {
                    Field& field = AddField(field_number, Field::FIELD_UINT32);
                    field._uint32s.push_back(ReadFromBytes<uint32_t>(current, remaining));
                } break;
                default: {
                    return false;
                } break;
            }
        }
        return true;
    }

    bool Message::GetSInt(uint32_t number, uint64_t& value) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            value = (field->_uint64s.front() >> 1) ^ (-(field->_uint64s.front() & 1));
            return true;
        }
        return false;
    }
    bool Message::GetVarInt(uint32_t number, uint64_t& value) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            value = field->_uint64s.front();
            return true;
        }
        return false;
    }
    bool Message::GetFixedInt32(uint32_t number, uint32_t& value) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT32);
            value = field->_uint32s.front();
            return true;
        }
        return false;
    }

    bool Message::GetFixedInt64(uint32_t number, uint64_t& value) {
        return GetVarInt(number, value);
    }

    bool Message::GetSFixedInt32(uint32_t number, uint32_t& value) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT32);
            value = static_cast<uint32_t>((field->_uint32s.front() >> 1) ^ (-(field->_uint32s.front() & 1)));
            return false;
        }
        return false;
    }

    bool Message::GetSFixedInt64(uint32_t number, uint64_t& value) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            value = (field->_uint64s.front() >> 1) ^ (-(field->_uint64s.front() & 1));
            return true;
        }
        return false;
    }

    bool Message::GetFloat(uint32_t number, float& f) {
        float ret = 0.0f;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT32);
            union { uint32_t i; float num; };
            i = field->_uint32s.front();
            f = num;
            return true;
        }
        return false;
    }

    bool Message::GetDouble(uint32_t number, double& db) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            union { uint64_t i; double num; };
            i = field->_uint64s.front();
            db = num;
            return true;
        }
        return false;
    }
    
    bool Message::GetString(uint32_t number, std::string& str) {
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_BYTES);
            assert(field->_bins.size() == 1);
            const binType& bin = field->_bins.front();
            str = std::string((const char*)bin.first, bin.second);
            return true;
        }
        return false;
    }

    Message* Message::GetMessage(uint32_t number) {
        Message* msg = NULL;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_BYTES);
            if (!field->_msgs.empty()) {
                assert( field->_msgs.size() == 1);
                msg = field->_msgs.front();
            } else {
                msg = new Message;
                assert(field->_bins.size() == 1);
                const binType& bin = field->_bins.front();
                if (msg->ParseFromBytes(bin.first, bin.second)) {
                    field->_msgs.push_back(msg);
                } else {
                    delete msg; msg = NULL;
                }
            }
        }
        return msg;
    }

    template<>
    bool Message::GetSIntArray(uint32_t number, std::vector<int32_t>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            for (uint32_t idx = 0; idx < field->_uint64s.size(); ++idx) {
                uint64_t item = field->_uint64s.at(idx);
                value.push_back(static_cast<int32_t>((item >> 1) ^ (-(item & 1))));
            }
            result = !field->_uint64s.empty();
        }
        return result;
    }

    template<>
    bool Message::GetSIntArray(uint32_t number, std::vector<int64_t>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            for (uint32_t idx = 0; idx < field->_uint64s.size(); ++idx) {
                uint64_t item = field->_uint64s.at(idx);
                value.push_back(static_cast<int64_t>((item >> 1) ^ (-(item & 1))));
            }
            result = !field->_uint64s.empty();
        }
        return result;
    }

    template<>
    bool Message::GetFixedInt32Array(uint32_t number, std::vector<int32_t>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT32);
            for (uint32_t idx = 0; idx < field->_uint32s.size(); ++idx) {
                value.push_back(static_cast<uint32_t>(field->_uint32s.at(idx)));
            }
            result = !field->_uint32s.empty();
        }
        return result;
    }

    template<>
    bool Message::GetFixedInt32Array(uint32_t number, std::vector<uint32_t>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT32);
            for (uint32_t idx = 0; idx < field->_uint32s.size(); ++idx) {
                value.push_back(field->_uint32s.at(idx));
            }
            result = !field->_uint32s.empty();
        }
        return result;
    }

    bool Message::GetFixedInt64Array(uint32_t number, std::vector<uint64_t>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            value.assign(field->_uint64s.begin(), field->_uint64s.end());
            result = !field->_uint64s.empty();
        }
        return result;
    }

    bool Message::GetFloatArray(uint32_t number, std::vector<float>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT32);
            union { uint32_t i; float f; };
            for (uint32_t idx = 0; idx < field->_uint64s.size(); ++idx) {
                i = static_cast<uint32_t>(field->_uint64s.at(idx));
                value.push_back(f);
            }
            result = !field->_uint32s.empty();
        }
        return result;
    }
    bool Message::GetDoubleArray(uint32_t number, std::vector<double>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_UINT64);
            union { uint64_t i; double db; };
            for (uint32_t idx = 0; idx < field->_uint64s.size(); ++idx) {
                i = static_cast<uint64_t>(field->_uint64s.at(idx));
                value.push_back(db);
            }
            result = !field->_uint64s.empty();
        }
        return result;
    }
    bool Message::GetStringArray(uint32_t number, std::vector<std::string>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_BYTES);
            for (uint32_t idx = 0; idx < field->_bins.size(); ++idx) {
                const binType& bin = field->_bins.at(idx);
                value.push_back(std::string((const char*)bin.first, bin.second));
            }
            result = !field->_bins.empty();
        }
        return result;
    }
    bool Message::GetByteArray(uint32_t number, std::vector<binType>& value) {
        bool result = false;
        if (Field* field = GetField(number)) {
            assert(field->type == Field::FIELD_BYTES);
            for (uint32_t idx = 0; idx < field->_bins.size(); ++idx) {
                value.push_back(field->_bins.at(idx));
            }
            result = !field->_bins.empty();
        }
        return result;
    }

    std::vector<Message*>* Message::GetMessageArray(uint32_t number) {
        if (Field* field = GetField(number)) {
            if (field->_msgs.empty()) {
                for (uint32_t idx = 0; idx < field->_bins.size(); ++idx) {
                    const binType& bin = field->_bins.at(idx);
                    Message* msg = new Message;
                    if (msg->ParseFromBytes(bin.first, bin.second)) {
                        field->_msgs.push_back(msg);
                    } else {
                        delete msg; msg = NULL;
                    }
                }
            }
            return &field->_msgs;
        }
        return NULL;
    }


    Field& Message::AddField(uint32_t number, enum Field::FieldType type) {
        Field* field = NULL;
        for (std::vector<Field>::iterator it = _fields.begin(); it != _fields.end(); ++it) {
            if (it->number == number) {
                field = &(*it);
            }
        }
        if (!field) {
            _fields.push_back(Field(number, type));
            field = &_fields.back();
        }
        return *field;
    }

    Field* Message::GetField(uint32_t number) {
        for (std::vector<Field>::iterator it = _fields.begin(); it != _fields.end(); ++it) {
            if (it->number == number) {
                return &*it;
            }
        }
        return NULL;
    }

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

}
