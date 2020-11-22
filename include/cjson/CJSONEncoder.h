#ifndef __CJSON_ENCOER_H__
#define __CJSON_ENCOER_H__

#include <string>
#include <map>
#include "struct2x.h"


struct cJSON;
namespace struct2x {

    class EXPORTAPI CJSONEncoder {
        cJSON* _root;
        cJSON* _cur;
        CJSONEncoder(const CJSONEncoder&);
        CJSONEncoder& operator=(const CJSONEncoder&);
    public:
        CJSONEncoder();
        ~CJSONEncoder();

        template<typename T>
        CJSONEncoder& operator&(serializeItem<T> value) {
            encodeValue(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value));
            return *this;
        }

        template<typename T>
        CJSONEncoder& operator << (const T& value) {
            const typename internal::TypeTraits<T>::Type& v = value;
            if (internal::TypeTraits<T>::isVector()) {
                operator << (v);
            } else {
                if (typename internal::TypeTraits<T>::Type* pValue = const_cast<typename internal::TypeTraits<T>::Type*>(&v)) {
                    internal::serializeWrapper(*this, *pValue);
                }
            }
            return *this;
        }

        template<typename K, typename V>
        CJSONEncoder& operator <<(const std::map<K, V>& value) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                encodeValue(internal::STOT::type<K>::tostr(it->first), item);
            }
            return *this;
        }

        template<typename T>
        CJSONEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            encodeValue(sz, *(typename internal::TypeTraits<T>::Type*)(&value));
            return *this;
        }

        bool toString(std::string& str, bool bUnformatted = true);
    private:
        template<typename T>
        void encodeValue(const char* sz, const T& value) {
            cJSON* curItem = cur();
            createObject(sz);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            cur(curItem);
        }

        template<typename T>
        void encodeValue(const char* sz, const std::vector<T>& value) {
            cJSON* curItem = cur();
            createArray(sz);
            int32_t size = (int32_t)value.size();
            for (int32_t i = 0; i < size; ++i) {
                cJSON* lastItem = cur();
                if (internal::TypeTraits<T>::isVector())
                    addArrayToArray();
                else
                    addItemToArray();
                const typename internal::TypeTraits<T>::Type& item = value.at(i);
                this->operator<<(item);
                cur(lastItem);
            }
            cur(curItem);
        }

        template<typename K, typename V>
        void encodeValue(const char* sz, const std::map<K, V>& value) {
            cJSON* curItem = cur();
            createObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                cJSON* lastItem = cur();
                const V& item = it->second;
                encodeValue(internal::STOT::type<K>::tostr(it->first), item);
                cur(lastItem);
            }
            cur(curItem);
        }

        void encodeValue(const char* sz, bool value);
        void encodeValue(const char* sz, uint32_t value);
        void encodeValue(const char* sz, int32_t value);
        void encodeValue(const char* sz, uint64_t value);
        void encodeValue(const char* sz, int64_t value);
        void encodeValue(const char* sz, float value);
        void encodeValue(const char* sz, double value);
        void encodeValue(const char* sz, const std::string& value);
        void encodeValue(const char* sz, const std::vector<bool>& value);
        void encodeValue(const char* sz, const std::vector<uint32_t>& value);
        void encodeValue(const char* sz, const std::vector<int32_t>& value);
        void encodeValue(const char* sz, const std::vector<uint64_t>& value);
        void encodeValue(const char* sz, const std::vector<int64_t>& value);
        void encodeValue(const char* sz, const std::vector<float>& value);
        void encodeValue(const char* sz, const std::vector<double>& value);
        void encodeValue(const char* sz, const std::vector<std::string>& value);

        CJSONEncoder& operator<<(const std::vector<int32_t>& value);
        CJSONEncoder& operator<<(const std::vector<float>& value);
        CJSONEncoder& operator<<(const std::vector<double>& value);
        CJSONEncoder& operator<<(const std::vector<std::string>& value);

        void createObject(const char* sz);
        void createArray(const char* sz);
        void addItemToArray();
        void addArrayToArray();
        void cur(cJSON* item) { _cur = item; }
        cJSON* cur() { return _cur; }
    };

}

#endif