#ifndef __PROTOBUF_DECODER_H__
#define __PROTOBUF_DECODER_H__

#include <assert.h>
#include <string>
#include <map>
#include "struct2x.h"

namespace struct2x {
    class PBDecoder;
}

namespace proto {

    typedef std::pair<const uint8_t*, size_t> bin_type;

    //TYPE_VARINT = 0,    // int32,int64,uint32,uint64,bool,enum
    template<typename T, typename P> struct convertVarint {
        static inline T value(const P& cValue) { return T(cValue); }
    };
    template<> struct convertVarint<float, uint32_t> {
        static inline float value(const uint32_t& cValue) { union { uint32_t i; float f; }; i = cValue; return f; }
    };
    template<> struct convertVarint<double, uint64_t> {
        static inline double value(const uint64_t& cValue) { union { uint64_t i; double db; }; i = cValue; return db; }
    };
    //TYPE_SVARINT = 1,   // sint32,sin64
    template<typename T, typename P> struct convertSvarint {
        static inline T value(const P& cValue) { return T((cValue >> 1) ^ (-(cValue & 1))); }
    };


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

    class EXPORTAPI Message {
        typedef void(*convert_t)(void*, const void*, const uint32_t, bool*);
        class converter {
            void convert(void*, const void*, const uint32_t, bool*);
            convert_t _func;
            void* _value;
            const uint32_t _type;
            bool* _pHas;
        public:
            converter(convert_t func, void* value, const uint32_t type, bool* pHas) :_func(func), _value(value), _type(type), _pHas(pHas) {}
            void operator()(const void* cValue) const { (*_func)(_value, cValue, _type, _pHas); }
        };

        const uint8_t* _sz;
        unsigned int _size;
        std::map<uint32_t, converter> _functionSet;

        static uint64_t ReadVarInt(const uint8_t*& current, size_t& remaining);
    public:
        Message(const uint8_t* sz, uint32_t size);

        const uint8_t* data() const { return _sz; }
        unsigned int size() const { return _size; }

        template<typename P, typename T>
        bool bind(void(*f)(T&, const P&, const uint32_t, bool*), struct2x::serializeItem<T>& value) {
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), &value.value, value.type, value.bHas))).second;
        }

        template<typename P, typename T>
        bool bind(void(*f)(std::vector<T>&, const P&, const uint32_t, bool*), struct2x::serializeItem<std::vector<T> >& value) {
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), &value.value, value.type, value.bHas))).second;
        }

        template<typename P, typename K, typename V>
        bool bind(void(*f)(std::map<K, V>&, const P&, const uint32_t, bool*), struct2x::serializeItem<std::map<K, V> >& value) {
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), &value.value, value.type, value.bHas))).second;
        }

        bool ParseFromBytes();

        template<typename T, typename P>
        static void convertValue(T& value, const P& cValue, const uint32_t type, bool* pHas) {
            if (type == struct2x::TYPE_VARINT)
                value = convertVarint<T, P>::value(cValue);
            else if (type == struct2x::TYPE_SVARINT) {
                value = convertSvarint<T, P>::value(cValue);
            } else {
                assert(false);
            }
            if (pHas) *pHas = true;
        }

        static void convertValue(std::string& value, const bin_type& cValue, const uint32_t type, bool* pHas) {
            value = std::string((const char*)cValue.first, cValue.second);
            if (pHas) *pHas = true;
        }

        template<typename T>
        static void convertCustom(T& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            struct2x::PBDecoder decoder(cValue.first, cValue.second);
            decoder >> value;
            if (pHas) *pHas = true;
        }

        template<typename T, typename P>
        static void convertArray(std::vector<T>& value, const P& cValue, const uint32_t type, bool* pHas) {
            if (type == struct2x::TYPE_VARINT)
                value.push_back(convertVarint<T, P>::value(cValue));
            else if (type == struct2x::TYPE_SVARINT) {
                value.push_back(convertSvarint<T, P>::value(cValue));
            } else {
                assert(false);
            }
            if (pHas) *pHas = true;
        }

        template<typename T>
        static void convertArray(std::vector<T>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            const uint8_t* current = cValue.first;
            size_t remaining = cValue.second;
            while (remaining) {
                const uint64_t varint = Message::ReadVarInt(current, remaining);
                value.push_back(static_cast<T>(varint));
            }
            if (pHas) *pHas = true;
        }

        static void convertArray(std::vector<std::string>& value, const bin_type& cValue, const uint32_t type, bool* pHas) {
            value.push_back(std::string((const char*)cValue.first, cValue.second));
            if (pHas) *pHas = true;
        }

        template<typename T>
        static void convertCustomArray(std::vector<T>& value, const bin_type& cValue, const uint32_t type, bool* pHas) {
            struct2x::PBDecoder decoder(cValue.first, cValue.second);
            typename T temp;
            decoder >> temp;
            value.push_back(temp);
            if (pHas) *pHas = true;
        }

        template<typename K, typename V>
        static void convertMap(std::map<K, V>& value, const bin_type& cValue, const uint32_t type, bool* pHas) {
            struct2x::PBDecoder decoder(cValue.first, cValue.second);
            typename K key = typename K();
            decoder.decodeValue(SERIALIZE(1, key));
            typename V v = typename V();
            decoder.decodeValue(SERIALIZE(2, v));
            decoder.ParseFromBytes();
            value.insert(std::pair<K, V>(key, v));
            if (pHas) *pHas = true;
        }

    };

}  // namespace proto

namespace struct2x {

    class EXPORTAPI PBDecoder {
        friend class proto::Message;
        proto::Message _msg;

        PBDecoder(const PBDecoder&);
        PBDecoder& operator=(const PBDecoder&);
    public:
        PBDecoder(const uint8_t* sz, uint32_t size);
        ~PBDecoder();

        template<typename T>
        bool operator>>(T& value) {
            internal::serializeWrapper(*this, value);
            return ParseFromBytes();
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<T> value) {
            decodeValue(value);
            return *this;
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<std::vector<T> > value) {
            if (!value.value.empty()) value.value.clear();
            decodeRepaeted(value);
            return *this;
        }

        template<typename K, typename V>
        PBDecoder& operator&(serializeItem<std::map<K, V> > value) {
            if (!value.value.empty()) value.value.clear();
            _msg.bind<proto::bin_type, K, V>(&proto::Message::convertMap, value);
            return *this;
        }
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<float, V> > value);
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<double, V> > value);
    private:
        template<typename T>
        void decodeValue(serializeItem<T>& v) {
            _msg.bind<proto::bin_type, T>(&proto::Message::convertCustom, v);
        }
        void decodeValue(serializeItem<bool>&);
        void decodeValue(serializeItem<int32_t>&);
        void decodeValue(serializeItem<uint32_t>&);
        void decodeValue(serializeItem<int64_t>&);
        void decodeValue(serializeItem<uint64_t>&);
        void decodeValue(serializeItem<float>&);
        void decodeValue(serializeItem<double>&);
        void decodeValue(serializeItem<std::string>&);

        template<typename T>
        void decodeRepaeted(serializeItem<std::vector<T> >& v) {
            _msg.bind<proto::bin_type, std::vector<T> >(&proto::Message::convertCustomArray, v);
        }
        void decodeRepaeted(serializeItem<std::vector<bool> >&);
        void decodeRepaeted(serializeItem<std::vector<int32_t> >&);
        void decodeRepaeted(serializeItem<std::vector<uint32_t> >&);
        void decodeRepaeted(serializeItem<std::vector<int64_t> >&);
        void decodeRepaeted(serializeItem<std::vector<uint64_t> >&);
        void decodeRepaeted(serializeItem<std::vector<float> >&);
        void decodeRepaeted(serializeItem<std::vector<double> >&);
        void decodeRepaeted(serializeItem<std::vector<std::string> >&);

        bool ParseFromBytes();
    };

}

#endif