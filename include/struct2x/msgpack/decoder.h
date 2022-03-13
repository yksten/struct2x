#ifndef __MSGPACK_DECODER_H__
#define __MSGPACK_DECODER_H__

#include <string>
#include <map>
#include <struct2x/traits.h>
#include <struct2x/struct2x.h>


#ifdef _MSC_VER
#ifdef EXPORTAPI
#define EXPORTAPI _declspec(dllimport)
#else
#define EXPORTAPI _declspec(dllexport)
#endif
#else
#define EXPORTAPI
#endif

namespace msgpack {

    typedef std::pair<const uint8_t*, size_t> bin_type;

    class EXPORTAPI Message {
        typedef bool(*convert_t)(void*, const void*, bool*);
        class converter {
            bool convert(void*, const void*, const uint32_t, bool*);
            convert_t _func;
            void* _value;
            bool* _pHas;
        public:
            converter(convert_t func, void* value, bool* pHas) :_func(func), _value(value), _pHas(pHas) {}
            bool operator()(const void* cValue) const { return (*_func)(_value, cValue, _pHas); }
        };

        const uint8_t* _sz;
        unsigned int _size;
        std::vector<std::pair<const char*, converter> > _functionSet;

    public:
        struct Value {
            enum { VALUENULL = -1, VALUEBOOL, VALUEINT, VALUEFLOAT, VALUEDOUBLE, VALUESTR };
            uint8_t type;
            union { bool b; int64_t i; float f; double db; };
            bin_type bin;
            Value() :type(VALUENULL), i(0) {}
        };

        Message(const uint8_t* sz, uint32_t size);

        static uint32_t getFieldSize(const uint8_t*& sz, size_t& size);
        static bool getFieldName(const uint8_t*& sz, size_t& size, msgpack::bin_type& result);
        static bool getValue(const uint8_t*& sz, size_t& size, Value& value, const Message::converter* pFunction = NULL);
        static bool doFunction(const Message::converter& pFunction, const Value& value);

        bool ParseFromBytes();

        template<typename P, typename T>
        void bind(bool(*f)(T&, const P&, bool*), const char* sz, T& value, bool* pHas) {
            _functionSet.push_back(std::pair<const char*, converter>(sz, converter(convert_t(f), &value, pHas)));
        }

        template<typename P, typename T>
        void bind(bool(*f)(std::vector<T>&, const P&, bool*), const char* sz, const std::vector<T>& value, bool* pHas) {
            _functionSet.push_back(std::pair<const char*, converter>(sz, converter(convert_t(f), &value, pHas)));
        }

        template<typename P, typename K, typename V>
        void bind(bool(*f)(std::map<K, V>&, const P&, bool*), const char* sz, const std::map<K, V>& value, bool* pHas) {
            _functionSet.push_back(std::pair<const char*, converter>(sz, converter(convert_t(f), &value, pHas)));
        }

    private:
        const Message::converter* getFunction(const char* sz, uint32_t size) const;
    };

}  // namespace msgpack

namespace struct2x {

    class EXPORTAPI MPDecoder {
        friend class msgpack::Message;
        msgpack::Message _msg;
        bool _bParseResult;

        MPDecoder(const MPDecoder&);
        MPDecoder& operator=(const MPDecoder&);
    public:
        MPDecoder(const uint8_t* sz, uint32_t size);

        template<typename T>
        bool operator>>(T& value) {
            internal::serializeWrapper(*this, value);
            return (_bParseResult && ParseFromBytes());
        }

        template<typename T>
        MPDecoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            decodeValue(sz, *(typename internal::TypeTraits<T>::Type*)(&value), pHas);
            return *this;
        }

        template<typename T>
        MPDecoder& operator&(serializeItem<T> value) {
            if (_bParseResult)
                _bParseResult = decodeValue(value.name, *(typename internal::TypeTraits<T>::Type*)(&value));
            return *this;
        }

        template<typename T>
        MPDecoder& operator&(serializeItem<std::vector<T> > value) {
            if (!value.value.empty()) value.value.clear();
            if (_bParseResult)
                _bParseResult = decodeValue(value.name, *(std::vector<typename internal::TypeTraits<T>::Type>*)(&value));
            return *this;
        }

        template<typename K, typename V>
        MPDecoder& operator&(serializeItem<std::map<K, V> > value) {
            if (!value.value.empty()) value.value.clear();
            _msg.bind<msgpack::bin_type, K, V>(&MPDecoder::convertMap, value);
            return *this;
        }
        template<typename V>
        MPDecoder& operator&(serializeItem<std::map<float, V> > value);
        template<typename V>
        MPDecoder& operator&(serializeItem<std::map<double, V> > value);
    private:
        template<typename T>
        void decodeValue(const char* sz, const T& v) {
            return _msg.bind<msgpack::bin_type, T>(&MPDecoder::convertCustom, v);
        }
        void decodeValue(const char* sz, bool &v, bool* pHas);
        void decodeValue(const char* sz, int32_t &v, bool* pHas);
        void decodeValue(const char* sz, uint32_t &v, bool* pHas);
        void decodeValue(const char* sz, int64_t &v, bool* pHas);
        void decodeValue(const char* sz, uint64_t &v, bool* pHas);
        void decodeValue(const char* sz, float &v, bool* pHas);
        void decodeValue(const char* sz, double &v, bool* pHas);
        void decodeValue(const char* sz, std::string& v, bool* pHas);

        template<typename T>
        bool decodeValue(const char* sz, std::vector<T>& v) {
            return _msg.bind<msgpack::bin_type, std::vector<T> >(&MPDecoder::convertCustomArray, v);
        }
        void decodeValue(const char* sz, std::vector<bool>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<int32_t>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<uint32_t>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<int64_t>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<uint64_t>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<float>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<double>& v, bool* pHas);
        void decodeValue(const char* sz, std::vector<std::string>& v, bool* pHas);

        bool ParseFromBytes();

        template<typename T, typename P>
        static bool convertValue(T& value, const P& cValue, bool* pHas) {
            value = cValue;
            if (pHas) *pHas = true;
            return true;
        }

        static bool convertValue(std::string& value, const msgpack::bin_type& cValue, bool* pHas) {
            value = std::string((const char*)cValue.first, cValue.second);
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T, typename P>
        static bool convertArray(std::vector<T>& value, const P& cValue, bool* pHas) {
            value.push_back(cValue);
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertArray(std::vector<T>& value, const msgpack::bin_type& cValue, bool* pHas) {
            const uint8_t* current = cValue.first;
            size_t remaining = cValue.second;
            //while (remaining) {
            //    uint64_t varint = 0;
            //    if (!msgpack::Message::ReadVarInt(current, remaining, varint))
            //        return false;
            //    value.push_back(static_cast<T>(varint));
            //}
            if (pHas) *pHas = true;
            return true;
        }

        static bool convertArray(std::vector<std::string>& value, const msgpack::bin_type& cValue, bool* pHas) {
            value.push_back(std::string((const char*)cValue.first, cValue.second));
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertCustom(T& value, const msgpack::bin_type& cValue, bool* pHas) {
            struct2x::MPDecoder decoder(cValue.first, cValue.second);
            if (!decoder.operator>>(value))
                return false;
            if (pHas) *pHas = true;
            return true;
        }

        template<typename T>
        static bool convertCustomArray(std::vector<T>& value, const msgpack::bin_type& cValue, bool* pHas) {
            struct2x::MPDecoder decoder(cValue.first, cValue.second);
            T temp = T();
            if (!decoder.operator>>(temp))
                return false;
            value.push_back(temp);
            if (pHas) *pHas = true;
            return true;
        }

        template<typename K, typename V>
        static bool convertMap(std::map<K, V>& value, const msgpack::bin_type& cValue, bool* pHas) {
            struct2x::MPDecoder decoder(cValue.first, cValue.second);
            K key = K();
            struct2x::serializeItem<K> kItem = SERIALIZATION(1, key);
            if (!decoder.decodeValue(*(serializeItem<typename internal::TypeTraits<K>::Type>*)(&kItem)))
                return false;
            V v = V();
            struct2x::serializeItem<V> vItem = SERIALIZATION(2, v);
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
