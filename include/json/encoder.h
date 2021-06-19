#ifndef __JSON_ENCODER_H__
#define __JSON_ENCODER_H__
#include "serialize.h"
#include <vector>
#include <map>
#include "thirdParty/json/GenericWriter.h"


namespace serialize {

    class EXPORTAPI JSONEncoder {
        custom::GenericWriter _writer;
    public:
        explicit JSONEncoder(std::string& str);
        ~JSONEncoder();

        template<typename T>
        JSONEncoder& operator&(serializeItem<T> value) {
            encodeValue(value.name, *static_cast<typename internal::TypeTraits<T>::Type*>(&value.value), value.bHas);
            return *this;
        }

        template<typename T>
        JSONEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            encodeValue(sz, *(const typename internal::TypeTraits<T>::Type*)&value, pHas);
            return *this;
        }

        template<typename T>
        bool operator << (const T& value) {
            const typename internal::TypeTraits<T>::Type& v = value;
            if (typename internal::TypeTraits<T>::Type* pValue = const_cast<typename internal::TypeTraits<T>::Type*>(&v)) {
                StartObject(NULL);
                internal::serializeWrapper(*this, *pValue);
                EndObject();
            }
            return _writer.result();
        }
        
        template<typename T>
        bool operator <<(const std::vector<T>& value) {
            StartArray(NULL);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                const typename internal::TypeTraits<T>::Type& item = value.at(i);
                encodeValue(NULL, item, NULL);
            }
            EndArray();
            return _writer.result();
        }

        template<typename K, typename V>
        bool operator <<(const std::map<K, V>& value) {
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
        void encodeValue(const char* sz, const T& value, bool* pHas) {
            StartObject(sz);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            EndObject();
        }

        template<typename T>
        void encodeValue(const char* sz, const std::vector<T>& value, bool* pHas) {
            StartArray(sz);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                const typename internal::TypeTraits<T>::Type& item = value.at(i);
                if (i) _writer.Separation();
                this->operator<<(item);
            }
            EndArray();
        }
        template<typename K, typename V>
        void encodeValue(const char* sz, const std::map<K, V>& value, bool* pHas) {
            StartObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            EndObject();
        }

        void encodeValue(const char* sz, bool value, bool* pHas);
        void encodeValue(const char* sz, uint32_t value, bool* pHas);
        void encodeValue(const char* sz, int32_t value, bool* pHas);
        void encodeValue(const char* sz, uint64_t value, bool* pHas);
        void encodeValue(const char* sz, int64_t value, bool* pHas);
        void encodeValue(const char* sz, float value, bool* pHas);
        void encodeValue(const char* sz, double value, bool* pHas);
        void encodeValue(const char* sz, const std::string& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<bool>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<uint32_t>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<int32_t>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<uint64_t>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<int64_t>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<float>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<double>& value, bool* pHas);
        void encodeValue(const char* sz, const std::vector<std::string>& value, bool* pHas);
        void StartObject(const char* sz);
        void EndObject();
        void StartArray(const char* sz);
        void EndArray();
    };

}


#endif
