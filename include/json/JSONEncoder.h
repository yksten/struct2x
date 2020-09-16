#ifndef __JSON_ENCOER_H__
#define __JSON_ENCOER_H__

#include <string>
#include <map>
#include "struct2x.h"


struct cJSON;
namespace struct2x {

    class EXPORTAPI JSONEncoder {
        cJSON* _root;
        cJSON* _cur;
        JSONEncoder(const JSONEncoder&);
        JSONEncoder& operator=(const JSONEncoder&);
    public:
        JSONEncoder();
        ~JSONEncoder();

        template<typename T>
        JSONEncoder& operator&(serializeItem<T> value) {
            return getValue(value.name, *(internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        JSONEncoder& operator << (const T& value) {
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
        JSONEncoder& operator <<(const std::map<K, V>& value) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                getValue(internal::STOT::type<K>::tostr(it->first), item, NULL);
            }
            return *this;
        }

        template<typename T>
        JSONEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            return getValue(sz, *(internal::TypeTraits<T>::Type*)(&value), pHas);
        }

        bool toString(std::string& str, bool bUnformatted = true);
    private:
        template<typename T>
        JSONEncoder& getValue(const char* sz, const T& value, bool* pHas) {
            cJSON* curItem = cur();
            createObject(sz);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            cur(curItem);
            return *this;
        }

        template<typename T>
        JSONEncoder& getValue(const char* sz, const std::vector<T>& value, bool* pHas) {
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
            return *this;
        }

        template<typename K, typename V>
        JSONEncoder& getValue(const char* sz, const std::map<K, V>& value, bool* pHas) {
            cJSON* curItem = cur();
            createObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                cJSON* lastItem = cur();
                const V& item = it->second;
                getValue(internal::STOT::type<K>::tostr(it->first), item, NULL);
                cur(lastItem);
            }
            cur(curItem);
            return *this;
        }

        JSONEncoder& getValue(const char* sz, bool value, bool* pHas);
        JSONEncoder& getValue(const char* sz, uint32_t value, bool* pHas);
        JSONEncoder& getValue(const char* sz, int32_t value, bool* pHas);
        JSONEncoder& getValue(const char* sz, uint64_t value, bool* pHas);
        JSONEncoder& getValue(const char* sz, int64_t value, bool* pHas);
        JSONEncoder& getValue(const char* sz, float value, bool* pHas);
        JSONEncoder& getValue(const char* sz, double value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::string& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<bool>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<uint32_t>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<int32_t>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<uint64_t>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<int64_t>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<float>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<double>& value, bool* pHas);
        JSONEncoder& getValue(const char* sz, const std::vector<std::string>& value, bool* pHas);

        JSONEncoder& operator<<(const std::vector<int32_t>& value);
        JSONEncoder& operator<<(const std::vector<float>& value);
        JSONEncoder& operator<<(const std::vector<double>& value);
        JSONEncoder& operator<<(const std::vector<std::string>& value);

        void createObject(const char* sz);
        void createArray(const char* sz);
        void addItemToArray();
        void addArrayToArray();
        void cur(cJSON* item) { _cur = item; }
        cJSON* cur() { return _cur; }
    };

}

#endif