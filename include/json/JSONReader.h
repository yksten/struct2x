#ifndef __JSON_READER_H__
#define __JSON_READER_H__
#include <string>
#include <map>
#include "struct2x.h"


struct cJSON;
namespace struct2x {

    class EXPORTAPI JSONReader {
        cJSON* _root;
        cJSON* _cur;
        JSONReader(const JSONReader&);
        JSONReader& operator=(const JSONReader&);
    public:
        JSONReader();
        ~JSONReader();

        template<typename T>
        JSONReader& operator&(serializeItem<T> value) {
            return convert(value.name(), value.value());
        }

        template<typename T>
        JSONReader& operator << (const T& value) {
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

        template<typename T>
        JSONReader& convert(const char* sz, const T& value) {
            cJSON* curItem = cur();
            createObject(sz);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            cur(curItem);
            return *this;
        }

        template<typename T>
        JSONReader& convert(const char* sz, const std::vector<T>& value) {
            cJSON* curItem = cur();
            createArray(sz);
            int size = (int)value.size();
            for (int i = 0; i < size; ++i) {
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
        JSONReader& convert(const char* sz, const std::map<K, V>& value) {
            cJSON* curItem = cur();
            createObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                cJSON* lastItem = cur();
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
                cur(lastItem);
            }
            cur(curItem);
            return *this;
        }

        template<typename K, typename V>
        JSONReader& operator <<(const std::map<K, V>& value) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            return *this;
        }

        bool toString(std::string& str, bool bUnformatted = false);
        JSONReader& convert(const char* sz, int value);
        JSONReader& convert(const char* sz, float value);
        JSONReader& convert(const char* sz, double value);
        JSONReader& convert(const char* sz, unsigned int value);
        JSONReader& convert(const char* sz, const char* value);
        JSONReader& convert(const char* sz, const std::string& value);
        JSONReader& convert(const char* sz, bool value);
        JSONReader& convert(const char* sz, const std::vector<int>& value);
        JSONReader& convert(const char* sz, const std::vector<float>& value);
        JSONReader& convert(const char* sz, const std::vector<double>& value);
        JSONReader& convert(const char* sz, const std::vector<std::string>& value);
    private:
        JSONReader& operator<<(const std::vector<int>& value);
        JSONReader& operator<<(const std::vector<float>& value);
        JSONReader& operator<<(const std::vector<double>& value);
        JSONReader& operator<<(const std::vector<std::string>& value);

        void createObject(const char* sz);
        void createArray(const char* sz);
        void addItemToArray();
        void addArrayToArray();
        void cur(cJSON* item) { _cur = item; }
        cJSON* cur() { return _cur; }
    };

}

#endif