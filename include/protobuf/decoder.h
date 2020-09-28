#ifndef __PROTOBUF_DECODER_H__
#define __PROTOBUF_DECODER_H__

#include <string>
#include <map>
#include "struct2x.h"


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


    class EXPORTAPI Message {
        typedef bool(*convert_t)(void*, const void*, const uint32_t, bool*);
        class converter {
            bool convert(void*, const void*, const uint32_t, bool*);
            convert_t _func;
            void* _value;
            const uint32_t _type;
            bool* _pHas;
        public:
            converter(convert_t func, void* value, const uint32_t type, bool* pHas) :_func(func), _value(value), _type(type), _pHas(pHas) {}
            bool operator()(const void* cValue) const { return (*_func)(_value, cValue, _type, _pHas); }
        };

        const uint8_t* _sz;
        unsigned int _size;
        std::map<uint32_t, converter> _functionSet;

    public:
        Message(const uint8_t* sz, uint32_t size);

        const uint8_t* data() const { return _sz; }
        unsigned int size() const { return _size; }

        static bool ReadVarInt(const uint8_t*& current, size_t& remaining, uint64_t& result);
        bool ParseFromBytes();

        template<typename P, typename T>
        bool bind(bool(*f)(T&, const P&, const uint32_t, bool*), struct2x::serializeItem<T>& value) {
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), &value.value, value.type, value.bHas))).second;
        }

        template<typename P, typename T>
        bool bind(bool(*f)(std::vector<T>&, const P&, const uint32_t, bool*), struct2x::serializeItem<std::vector<T> >& value) {
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), &value.value, value.type, value.bHas))).second;
        }

        template<typename P, typename K, typename V>
        bool bind(bool(*f)(std::map<K, V>&, const P&, const uint32_t, bool*), struct2x::serializeItem<std::map<K, V> >& value) {
            return _functionSet.insert(std::pair<uint32_t, converter>(value.num, converter(convert_t(f), &value.value, value.type, value.bHas))).second;
        }

    };

}  // namespace proto

namespace struct2x {

    class EXPORTAPI PBDecoder {
        friend class proto::Message;
        proto::Message _msg;
        bool _bParseResult;

        PBDecoder(const PBDecoder&);
        PBDecoder& operator=(const PBDecoder&);
    public:
        PBDecoder(const uint8_t* sz, uint32_t size);

        template<typename T>
        bool operator>>(T& value) {
            internal::serializeWrapper(*this, value);
            return (_bParseResult && ParseFromBytes());
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
            _msg.bind<proto::bin_type, K, V>(&PBDecoder::convertMap, value);
            return *this;
        }
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<float, V> > value);
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<double, V> > value);
    private:
        template<typename T>
        bool decodeValue(serializeItem<T>& v) {
            return _msg.bind<proto::bin_type, T>(&PBDecoder::convertCustom, v);
        }
        bool decodeValue(serializeItem<bool>&);
        bool decodeValue(serializeItem<int32_t>&);
        bool decodeValue(serializeItem<uint32_t>&);
        bool decodeValue(serializeItem<int64_t>&);
        bool decodeValue(serializeItem<uint64_t>&);
        bool decodeValue(serializeItem<float>&);
        bool decodeValue(serializeItem<double>&);
        bool decodeValue(serializeItem<std::string>&);

        template<typename T>
        bool decodeRepaeted(serializeItem<std::vector<T> >& v) {
            return _msg.bind<proto::bin_type, std::vector<T> >(&PBDecoder::convertCustomArray, v);
        }
        bool decodeRepaeted(serializeItem<std::vector<bool> >&);
        bool decodeRepaeted(serializeItem<std::vector<int32_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<uint32_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<int64_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<uint64_t> >&);
        bool decodeRepaeted(serializeItem<std::vector<float> >&);
        bool decodeRepaeted(serializeItem<std::vector<double> >&);
        bool decodeRepaeted(serializeItem<std::vector<std::string> >&);

        bool ParseFromBytes();

        template<typename T, typename P>
        static bool convertValue(T& value, const P& cValue, const uint32_t type, bool* pHas) {
            if (type == struct2x::TYPE_VARINT)
                value = proto::convertVarint<T, P>::value(cValue);
            else if (type == struct2x::TYPE_SVARINT) {
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
            if (type == struct2x::TYPE_VARINT)
                value.push_back(proto::convertVarint<T, P>::value(cValue));
            else if (type == struct2x::TYPE_SVARINT) {
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
            struct2x::PBDecoder decoder(cValue.first, cValue.second);
            if (!decoder.operator>>(value))
                return false;
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertCustomArray(std::vector<T>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            struct2x::PBDecoder decoder(cValue.first, cValue.second);
            T temp = T();
            if (!decoder.operator>>(temp))
                return false;
            value.push_back(temp);
            if (pHas) *pHas = true;
            return true;
        }

        template<typename K, typename V>
        static bool convertMap(std::map<K, V>& value, const proto::bin_type& cValue, const uint32_t type, bool* pHas) {
            struct2x::PBDecoder decoder(cValue.first, cValue.second);
            K key = K();
            struct2x::serializeItem<K> kItem = SERIALIZE(1, key);
            if (!decoder.decodeValue(*(serializeItem<typename internal::TypeTraits<K>::Type>*)(&kItem)))
                return false;
            V v = V();
            struct2x::serializeItem<V> vItem = SERIALIZE(2, v);
            if (!decoder.decodeValue(*(serializeItem<typename internal::TypeTraits<V>::Type>*)(&vItem)))
                return false;
            if (!decoder.ParseFromBytes())
                return false;
            value.insert(std::pair<K, V>(key, v));
            if (pHas) *pHas = true;
            return true;
        }
    };

}

#endif