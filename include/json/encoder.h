#ifndef __JSON_ENCODER_H__
#define __JSON_ENCODER_H__
#include "serialize.h"
#include <vector>
#include <map>
#include "json/GenericWriter.h"


namespace serialize {

    class JSONEncoder {
        custom::GenericWriter _writer;
    public:
        explicit JSONEncoder(std::string& str) : _writer(str) {}

        template<typename T>
        FORCEINLINE JSONEncoder& operator&(serializeItem<T> value) {
            return convert(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        FORCEINLINE JSONEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            encodeValue(sz, *(const typename internal::TypeTraits<T>::Type*)(&value));
            return *this;
        }

        template<typename T>
        FORCEINLINE bool operator << (const T& value) {
            StartObject(NULL);
            internal::serializeWrapper(*this, *const_cast<typename internal::TypeTraits<T>::Type*>((const typename internal::TypeTraits<T>::Type*)&value));
            EndObject();
            return _writer.result();
        }
        
        template<typename T>
        FORCEINLINE bool operator <<(const std::vector<T>& value) {
            StartArray(NULL);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                const typename internal::TypeTraits<T>::Type& item = value.at(i);
                encodeValue(NULL, item);
            }
            EndArray();
            return _writer.result();
        }

        template<typename K, typename V>
        FORCEINLINE bool operator <<(const std::map<K, V>& value) {
            StartObject(NULL);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                typename internal::TypeTraits<K>::Type* pKey = const_cast<typename internal::TypeTraits<K>::Type*>(&(it->first));
                typename internal::TypeTraits<V>::Type* pValue = const_cast<typename internal::TypeTraits<V>::Type*>(&(it->second));
                convert(internal::STOT::type<typename internal::TypeTraits<K>::Type>::tostr(*pKey), *pValue);
            }
            EndObject();
            return _writer.result();
        }
    private:
        template<typename T>
        FORCEINLINE void encodeValue(const char* sz, const T& value) {
            StartObject(sz);
            internal::serializeWrapper(*this, *const_cast<typename internal::TypeTraits<T>::Type*>((const typename internal::TypeTraits<T>::Type*)(&value)));
            EndObject();
        }

        template<typename T>
        FORCEINLINE void encodeValue(const char* sz, const std::vector<T>& value) {
            StartArray(sz);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                const typename internal::TypeTraits<T>::Type& item = value.at(i);
                if (i) _writer.Separation();
                encodeValue(NULL, item);
            }
            EndArray();
        }
        
        template<typename K, typename V>
        FORCEINLINE void encodeValue(const char* sz, const std::map<K, V>& value) {
            StartObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            EndObject();
        }
        
        FORCEINLINE void encodeValue(const char* sz, const bool& value) {
            _writer.Key(sz).Bool(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const uint32_t& value) {
            _writer.Key(sz).Uint64(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const int32_t& value) {
            _writer.Key(sz).Int64(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const uint64_t& value) {
            _writer.Key(sz).Uint64(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const int64_t& value) {
            _writer.Key(sz).Int64(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const float& value) {
            _writer.Key(sz).Double(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const double& value) {
            _writer.Key(sz).Double(value);
        }
        FORCEINLINE void encodeValue(const char* sz, const std::string& value) {
            _writer.Key(sz).String(value.c_str());
        }
        
        FORCEINLINE void encodeValue(const char* sz, const std::vector<bool>& value) {
            StartArray(sz);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                const bool item = value.at(i);
                encodeValue(NULL, item);
            }
            EndArray();
        }
        
        FORCEINLINE void StartObject(const char* sz) {
            _writer.Key(sz).StartObject();
        }

        FORCEINLINE void EndObject() {
            _writer.EndObject();
        }

        FORCEINLINE void StartArray(const char* sz) {
            _writer.Key(sz).StartArray();
        }

        FORCEINLINE void EndArray() {
            _writer.EndArray();
        }
    };

}


#endif
