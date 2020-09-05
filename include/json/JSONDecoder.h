#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__
#include <string>
#include <map>
#include "struct2x.h"


struct cJSON;
namespace struct2x {

    class EXPORTAPI JSONDecoder {
        cJSON* _root;
        cJSON* _cur;
        JSONDecoder();
        JSONDecoder(const JSONDecoder&);
        JSONDecoder& operator=(const JSONDecoder&);
    public:
        JSONDecoder(const char* sz);
        ~JSONDecoder();

        template<typename T>
        bool operator >> (T& value) {
            if (_cur) {
                internal::serializeWrapper(*this, value);
            }
            return (_cur) ? true : false;
        }

        template<typename T>
        JSONDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, value.value);
        }

        template<typename T>
        JSONDecoder& convert(const char* sz, T& value, bool* pHas = NULL) {
            cJSON* curItem = cur();
            if (getObject(sz)) {
                internal::serializeWrapper(*this, value);
                if (pHas) *pHas = true;
            }
            cur(curItem);
            return *this;
        }

        template<typename T>
        JSONDecoder& convert(const char* sz, std::vector<T>& value, bool* pHas = NULL) {
            cJSON* curItem = cur();
            if (getObject(sz)) {
                int32_t size = getArraySize();
                if (size&& !value.empty())
                    value.clear();
                for (int32_t i = 0; i < size; ++i) {
                    cJSON* lastItem = cur();
                    getArrayItem(i);
                    typename internal::TypeTraits<T>::Type item;
                    this->operator>>(item);
                    value.push_back(item);
                    cur(lastItem);
                }
                if (pHas) *pHas = true;
            }
            cur(curItem);
            return *this;
        }
        template<typename K, typename V>
        JSONDecoder& convert(const char* sz, std::map<K, V>& value, bool* pHas = NULL) {
            cJSON* curItem = cur();
            if (getObject(sz)) {
                int32_t size = getMapSize();
                if (size&& !value.empty())
                    value.clear();
                for (int32_t i = 0; i < size; ++i) {
                    cJSON* lastItem = cur();
                    std::string key;
                    V item = V();
                    getkeyValue(i, key, item);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                    cur(lastItem);
                }
                if (pHas) *pHas = true;
            }
            cur(curItem);
            return *this;
        }

        template<typename T>
        bool operator >>(std::vector<T>& value) {
            if (_cur) {
                int32_t size = getArraySize();
                if (size && !value.empty())
                    value.clear();
                for (int32_t i = 0; i < size; ++i) {
                    cJSON* lastItem = cur();
                    getArrayItem(i);
                    T item;
                    this->operator>>(item);
                    value.push_back(item);
                    cur(lastItem);
                }
            }
            return (_cur) ? true : false;
        }

        template<typename K, typename V>
        bool operator >>(std::map<K, V>& value) {
            if (_cur) {
                int32_t size = getMapSize();
                if (size && !value.empty())
                    value.clear();
                for (int32_t i = 0; i < size; ++i) {
                    std::string key;
                    V item = V();
                    getkeyValue(i, key, item);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                }
            }
            return (_cur) ? true : false;
        }
        JSONDecoder& convert(const char* sz, bool& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, uint32_t& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, int32_t& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, uint64_t& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, int64_t& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, float& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, double& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::string& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<bool>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<uint32_t>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<int32_t>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<uint64_t>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<int64_t>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<float>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<double>& value, bool* pHas = NULL);
        JSONDecoder& convert(const char* sz, std::vector<std::string>& value, bool* pHas = NULL);
    private:
        JSONDecoder& operator >>(std::vector<int32_t>& value);
        JSONDecoder& operator >>(std::vector<float>& value);
        JSONDecoder& operator >>(std::vector<double>& value);
        JSONDecoder& operator >>(std::vector<std::string>& value);

        bool getObject(const char* sz);
        int32_t getArraySize()const;
        void getArrayItem(int32_t i);
        int32_t getMapSize() const;
        const char* getChildName(int32_t i)const;
        template<typename T>
        void getkeyValue(int32_t i, std::string& key, T& value) {
            key = getChildName(i);
            convert(key.c_str(), value);
        }
        void cur(cJSON* item) { _cur = item; }
        cJSON* cur() { return _cur; }
    };

}

#endif