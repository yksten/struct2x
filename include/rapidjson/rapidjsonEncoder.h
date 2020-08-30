#ifndef __RAPIDJSON_ENCODER_H__
#define __RAPIDJSON_ENCODER_H__
#include "struct2x.h"
#include <string>
#include <map>


namespace struct2x {

    class writerWrapper;

    class EXPORTAPI rapidjsonEncoder {
        writerWrapper* _ptr;
        writerWrapper& _writer;
    public:
        rapidjsonEncoder();
        ~rapidjsonEncoder();

        bool toString(std::string& str);

        template<typename T>
        rapidjsonEncoder& operator&(serializeItem<T> value) {
            return convert(value.name, value.value);
        }

        template<typename T>
        rapidjsonEncoder& operator << (const T& value) {
            const typename internal::TypeTraits<T>::Type& v = value;
            if (internal::TypeTraits<T>::isVector()) {
                operator << (v);
            } else {
                if (typename internal::TypeTraits<T>::Type* pValue = const_cast<typename internal::TypeTraits<T>::Type*>(&v)) {
                    StartObject(NULL);
                    internal::serializeWrapper(*this, *pValue);
                    EndObject();
                }
            }
            return *this;
        }

        template<typename T>
        rapidjsonEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            StartObject(sz);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            EndObject();
            return *this;
        }

        template<typename T>
        rapidjsonEncoder& convert(const char* sz, const std::vector<T>& value, bool* pHas = NULL) {
            StartArray(sz);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                const typename internal::TypeTraits<T>::Type& item = value.at(i);
                this->operator<<(item);
            }
            EndArray();
            return *this;
        }
        template<typename K, typename V>
        rapidjsonEncoder& convert(const char* sz, const std::map<K, V>& value, bool* pHas = NULL) {
            StartObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            EndObject();
            return *this;
        }

        template<typename K, typename V>
        rapidjsonEncoder& operator <<(const std::map<K, V>& value) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            return *this;
        }

        rapidjsonEncoder& convert(const char* sz, bool value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, uint32_t value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, int32_t value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, uint64_t value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, int64_t value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, float value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, double value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::string& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<bool>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<uint32_t>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<int32_t>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<uint64_t>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<int64_t>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<float>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<double>& value, bool* pHas = NULL);
        rapidjsonEncoder& convert(const char* sz, const std::vector<std::string>& value, bool* pHas = NULL);
    private:
        void StartObject(const char* sz);
        void EndObject();
        void StartArray(const char* sz);
        void EndArray();
    };
}


#endif