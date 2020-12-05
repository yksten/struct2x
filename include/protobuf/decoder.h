#ifndef __PROTOBUF_DECODER_H__
#define __PROTOBUF_DECODER_H__

#include <string>
#include <map>
#include "serialize.h"


namespace proto {

    typedef size_t offset_type;

    typedef std::pair<const uint8_t*, size_t> bin_type;

    //TYPE_VARINT = 0,    // int32,int64,uint32,uint64,bool,enum
    template<typename T, typename P> struct convertVarint {
        static inline T value(const P& cValue) { return static_cast<T>(cValue); }
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


    class EXPORTAPI Message {
        typedef bool(*convert_t)(void*, const void*, const uint32_t, bool*);
        class converter {
            convert_t _func;
            offset_type _offset;
            const uint32_t _type;
            bool* _pHas;
        public:
            converter(convert_t func, offset_type offset, const uint32_t type, bool* pHas) :_func(func), _offset(offset), _type(type), _pHas(pHas) {}
            bool operator()(uint8_t* pStruct, const void* cValue) const { return (*_func)(pStruct + _offset, cValue, _type, _pHas); }
            void offset(offset_type offset) { _offset = offset; }
        };

        uint8_t* _struct;
        std::map<uint32_t, converter> _functionSet;

    public:
        Message();

        bool empty() const { return _functionSet.empty(); }
        void offset(uint32_t field_number, offset_type n);

        void setStruct(void* pStruct);
        void call(uint32_t field_number, const void* cValue) const;

        static bool ReadVarInt(const uint8_t*& current, size_t& remaining, uint64_t& result);
        bool ParseFromBytes(const uint8_t* sz, uint32_t size);

        template<typename P, typename T>
        bool bind(bool(*f)(T&, const P&, const uint32_t, bool*), serialize::serializeItem<T>& value) {
            offset_type offset = ((uint8_t*)(&value.value)) - _struct;
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), offset, value.type, value.bHas))).second;
        }

        template<typename P, typename T>
        bool bind(bool(*f)(std::vector<T>&, const P&, const uint32_t, bool*), serialize::serializeItem<std::vector<T> >& value) {
            offset_type offset = ((uint8_t*)(&value.value)) - _struct;
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), offset, value.type, value.bHas))).second;
        }

        template<typename P, typename K, typename V>
        bool bind(bool(*f)(std::map<K, V>&, const P&, const uint32_t, bool*), serialize::serializeItem<std::map<K, V> >& value) {
            offset_type offset = ((uint8_t*)(&value.value)) - _struct;
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), offset, value.type, value.bHas))).second;
        }

    };

}  // namespace proto

namespace serialize {

    class EXPORTAPI PBDecoder {
        friend class proto::Message;
        proto::Message* _msg;
        const uint8_t* _sz;
        unsigned int _size;
        bool _bParseResult;

        PBDecoder(const PBDecoder&);
        PBDecoder& operator=(const PBDecoder&);
        PBDecoder(const uint8_t* sz, uint32_t size);
    public:
        explicit PBDecoder(const std::string& str);

        template<typename T>
        proto::Message getMessage(T& value) {
            proto::Message msg;
            _msg = &msg;
            msg.setStruct(&value);
            internal::serializeWrapper(*this, value);
            return msg;
        }

        template<typename T>
        bool operator>>(T& value) {
            static proto::Message msg = getMessage(value);
            msg.setStruct(&value);
            _bParseResult = false;
            internal::serializeWrapper(*this, value);
            _bParseResult = true;
            return msg.ParseFromBytes(_sz, _size);
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<T> value) {
            if (_bParseResult)
                _bParseResult = decodeValue(*(serializeItem<typename internal::TypeTraits<T>::Type>*)(&value));
            return *this;
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<std::vector<T> > value) {
            if (!value.value.empty()) value.value.clear();
            if (_bParseResult)
                _bParseResult = decodeRepaeted(*(serializeItem<std::vector<typename internal::TypeTraits<T>::Type> >*)(&value));
            return *this;
        }

        template<typename K, typename V>
        PBDecoder& operator&(serializeItem<std::map<K, V> > value) {
            if (!value.value.empty()) value.value.clear();
            if (_bParseResult)
                _bParseResult = _msg->bind<proto::bin_type, K, V>(&PBDecoder::convertMap, value);
            return *this;
        }
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<float, V> > value);
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<double, V> > value);
    private:
        template<typename T>
        bool decodeValue(serializeItem<T>& v) {
            return _msg->bind<proto::bin_type, T>(&PBDecoder::convertCustom, v);
        }
        bool decodeValue(serializeItem<bool>&);
        bool decodeValue(serializeItem<int32_t>&);
        bool decodeValue(serializeItem<uint32_t>&);
        bool decodeValue(serializeItem<int64_t>&);
        bool decodeValue(serializeItem<uint64_t>&);
        bool decodeValue(serializeItem<float>&);
        bool decodeValue(serializeItem<double>&);
        bool decodeValue(serializeItem<std::string>&);

        bool decodeRepaeted(serializeItem<std::vector<bool> >&);
        bool decodeRepaeted(serializeItem<std::vector<int32_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<uint32_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<int64_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<uint64_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<float> >&);
        bool decodeRepaeted(serializeItem<std::vector<double> >&);
        bool decodeRepaeted(serializeItem<std::vector<std::string> >&);

        template<typename T>
        bool decodeRepaeted(serializeItem<std::vector<T> >& v) {
            return _msg->bind<proto::bin_type, std::vector<T> >(&PBDecoder::convertCustomArray, v);
        }

        template<typename T, typename P>
        static bool convertValue(T& value, const P& cValue, const uint32_t type, bool* pHas) {
            if (type == serialize::TYPE_VARINT)
                value = proto::convertVarint<T, P>::value(cValue);
            else if (type == serialize::TYPE_SVARINT) {
                value = proto::convertSvarint<T, P>::value(cValue);
            } else {
                return false;
            }
            if (pHas) *pHas = true;
            return true;
        }

        static bool convertValue(std::string& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            value = std::string((const char*)cValue.first, cValue.second);
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T, typename P>
        static bool convertArray(std::vector<T>& value, const P& cValue, const uint32_t type, bool* pHas) {
            if (type == serialize::TYPE_VARINT)
                value.push_back(proto::convertVarint<T, P>::value(cValue));
            else if (type == serialize::TYPE_SVARINT) {
                value.push_back(proto::convertSvarint<T, P>::value(cValue));
            } else {
                return false;
            }
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertArray(std::vector<T>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            const uint8_t* current = cValue.first;
            size_t remaining = cValue.second;
            while (remaining) {
                uint64_t varint = 0;
                if (!proto::Message::ReadVarInt(current, remaining, varint))
                    return false;
                value.push_back(static_cast<T>(varint));
            }
            if (pHas) *pHas = true;
            return true;
        }

        static bool convertArray(std::vector<std::string>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            value.push_back(std::string((const char*)cValue.first, cValue.second));
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertCustom(T& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            serialize::PBDecoder decoder(cValue.first, cValue.second);
            if (!decoder.operator>>(value))
                return false;
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertCustomArray(std::vector<T>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            serialize::PBDecoder decoder(cValue.first, cValue.second);
            T temp = T();
            if (!decoder.operator>>(temp))
                return false;
            value.push_back(temp);
            if (pHas) *pHas = true;
            return true;
        }

        template<typename K, typename V>
        static bool convertMap(std::map<K, V>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            serialize::PBDecoder decoder(cValue.first, cValue.second);
            static proto::Message msg;
            decoder._msg = &msg;
            K key = K();
            V v = V();
            if (msg.empty()) {
                serialize::serializeItem<K> kItem = SERIALIZATION(1, key);
                decoder.decodeValue(*(serializeItem<typename internal::TypeTraits<K>::Type>*)(&kItem));
                serialize::serializeItem<V> vItem = SERIALIZATION(2, v);
                decoder.decodeValue(*(serializeItem<typename internal::TypeTraits<V>::Type>*)(&vItem));
            } else {
                msg.offset(1, (proto::offset_type)((uint8_t*)&key - NULL));
                msg.offset(2, (proto::offset_type)((uint8_t*)&v - NULL));
            }
            if (!msg.ParseFromBytes(decoder._sz, decoder._size))
                return false;
            value.insert(std::pair<K, V>(key, v));
            if (pHas) *pHas = true;
            return true;
        }
    };

}

#endif