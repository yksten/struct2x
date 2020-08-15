#ifndef __PROTO_H__
#define __PROTO_H__
#include <cstdint>
#include <vector>

namespace proto {

    // These are defined in:
    // https://developers.google.com/protocol-buffers/docs/encoding
    enum WireType {
        WIRETYPE_VARINT = 0,                // int32,int64,uint32,uint64,sint32,sin64,bool,enum
        WIRETYPE_64BIT = 1,                 // fixed64,sfixed64,double
        WIRETYPE_LENGTH_DELIMITED = 2,      // string,bytes,embedded messages,packed repeated fields
        WIRETYPE_GROUP_START = 3,           // Groups(deprecated)
        WIRETYPE_GROUP_END = 4,             // Groups(deprecated)
        WIRETYPE_32BIT = 5,                 // fixed32,sfixed32,float
    };

    typedef std::pair<const uint8_t*, size_t> binType;

    class Message;
    struct Field {
        enum FieldType {
            FIELD_UNKNOW = -1,
            FIELD_UINT32 = 0,
            FIELD_UINT64 = 1,
            FIELD_BYTES = 2,
        };

        uint32_t number;
        Field::FieldType type;

        std::vector<uint32_t> _uint32s;
        std::vector<uint64_t> _uint64s;
        std::vector<binType> _bins;
        std::vector<Message*> _msgs;

        Field(uint32_t num, Field::FieldType t);
        ~Field();
    };

    class Message {
        std::vector<Field> _fields;
    public:
        Message();
        bool ParseFromBytes(const uint8_t* szBin, size_t nBinSize);

        bool GetSInt(uint32_t number, uint64_t& value);
        bool GetVarInt(uint32_t number, uint64_t& value);
        bool GetFixedInt32(uint32_t number, uint32_t& value);
        bool GetFixedInt64(uint32_t number, uint64_t& value);
        bool GetSFixedInt32(uint32_t number, uint32_t& value);
        bool GetSFixedInt64(uint32_t number, uint64_t& value);
        bool GetFloat(uint32_t number, float& f);
        bool GetDouble(uint32_t number, double& db);
        bool GetString(uint32_t number, std::string& str);
        Message* GetMessage(uint32_t number);

        template<typename T>
        bool GetSIntArray(uint32_t number, std::vector<T>& value);

        template<typename T>
        bool GetVarIntArray(uint32_t number, std::vector<T>& value) {
            bool result = false;
            if (Field* field = GetField(number)) {
                if (field->type == Field::FIELD_UINT64) {
                    for (uint32_t idx = 0; idx < field->_uint64s.size(); ++idx) {
                        value.push_back(static_cast<T>(field->_uint64s.at(idx)));
                    }
                    result = !field->_uint64s.empty();
                } else if (field->type == Field::FIELD_BYTES) {
                    // for repeated [packed=true]
                    for (uint32_t idx = 0; idx < field->_bins.size(); ++idx) {
                        const binType& bin = field->_bins.at(idx);
                        const uint8_t* current = bin.first;
                        size_t remaining = bin.second;
                        while (remaining) {
                            const uint64_t varint = ReadVarInt(current, remaining);
                            value.push_back(static_cast<T>(varint));
                        }
                    }
                    result = !field->_bins.empty();
                } else {
                    assert(false);
                }
            }
            return result;
        }

        template<typename T>
        bool GetFixedInt32Array(uint32_t number, std::vector<T>& value);
        bool GetFixedInt64Array(uint32_t number, std::vector<uint64_t>& value);
        template<typename T>
        bool GetSFixedInt32Array(uint32_t number, std::vector<T>& value) { return false; }
        template<typename T>
        bool GetSFixedInt64Array(uint32_t number, std::vector<T>& value) { return false; }
        bool GetFloatArray(uint32_t number, std::vector<float>& value);
        bool GetDoubleArray(uint32_t number, std::vector<double>& value);
        bool GetStringArray(uint32_t number, std::vector<std::string>& value);
        bool GetByteArray(uint32_t number, std::vector<binType>& value);
        std::vector<Message*>* GetMessageArray(uint32_t number);
    private:
        Field& AddField(uint32_t number, enum Field::FieldType type);
        Field* GetField(uint32_t number);
        static uint64_t ReadVarInt(const uint8_t*& current, size_t& remaining);
    };

}  // namespace proto

#endif
