#ifndef __JSON_ENCODER_H__
#define __JSON_ENCODER_H__

#include <map>
#include <struct2x/traits.h>
#include <struct2x/struct2x.h>
#include <struct2x/json/GenericWriter.h>


namespace struct2x {

    class EXPORTAPI JSONEncoder {
        custom::GenericWriter _writer;
    public:
        explicit JSONEncoder(std::string& str, bool formatted = false);

        template<typename T>
        JSONEncoder& operator&(const serializeItem<T>& value) {
            return convert(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        JSONEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            encodeValue(sz, *(const typename internal::TypeTraits<T>::Type*)(&value));
            return *this;
        }

        template<typename T>
        bool operator << (const T& value) {
            StartObject(NULL);
            internal::serializeWrapper(*this, *const_cast<typename internal::TypeTraits<T>::Type*>((const typename internal::TypeTraits<T>::Type*)&value));
            EndObject();
            return _writer.result();
        }
        
        template<typename T>
        bool operator <<(const std::vector<T>& value) {
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
        void encodeValue(const char* sz, const T& value) {
            StartObject(sz);
            internal::serializeWrapper(*this, *const_cast<typename internal::TypeTraits<T>::Type*>((const typename internal::TypeTraits<T>::Type*)(&value)));
            EndObject();
        }

        template<typename T>
        void encodeValue(const char* sz, const std::vector<T>& value) {
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
        void encodeValue(const char* sz, const std::map<K, V>& value) {
            StartObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            EndObject();
        }
        
        void encodeValue(const char* sz, const bool& value);
        void encodeValue(const char* sz, const uint32_t& value);
        void encodeValue(const char* sz, const int32_t& value);
        void encodeValue(const char* sz, const uint64_t& value);
        void encodeValue(const char* sz, const int64_t& value);
        void encodeValue(const char* sz, const float& value);
        void encodeValue(const char* sz, const double& value);
        void encodeValue(const char* sz, const std::string& value);
        void encodeValue(const char* sz, const std::vector<bool>& value);
        void StartObject(const char* sz);
        void EndObject();
        void StartArray(const char* sz);
        void EndArray();
    };

}


#endif
