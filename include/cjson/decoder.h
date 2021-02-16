#ifndef __CJSON_DECODER_H__
#define __CJSON_DECODER_H__

#include <string>
#include <map>
#include "serialize.h"


struct cJSON;
namespace serialize {

    class EXPORTAPI CJSONDecoder {
        cJSON* _root;
        cJSON* _cur;
        CJSONDecoder();
        CJSONDecoder(const CJSONDecoder&);
        CJSONDecoder& operator=(const CJSONDecoder&);
    public:
        explicit CJSONDecoder(const char* sz);
        ~CJSONDecoder();

        template<typename T>
        bool operator >> (T& value) {
            if (_cur) {
                internal::serializeWrapper(*this, value);
            }
            return (_cur) ? true : false;
        }

        template<typename T>
        CJSONDecoder& operator&(serializeItem<T> value) {
            decodeValue(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
            return *this;
        }

        template<typename T>
        CJSONDecoder& convert(const char* sz, T& value, bool* pHas = NULL) {
            decodeValue(sz, *(typename internal::TypeTraits<T>::Type*)(&value), pHas);
            return *this;
        }

        template<typename K, typename V>
        bool operator >>(std::map<K, V>& value) {
            if (_cur) {
                int32_t size = getMapSize();
                if (size && !value.empty())
                    value.clear();
                for (int32_t i = 0; i < size; ++i) {
                    std::string key = getChildName(i);
                    V item = V();
                    decodeValue(key.c_str(), item, NULL);
                    value.insert(std::pair<K, V>(static_cast<K>(internal::STOT::type<typename internal::TypeTraits<K>::Type>::strto(key.c_str())), item));
                }
            }
            return (_cur) ? true : false;
        }

    private:
        template<typename T>
        void decodeValue(const char* sz, T& value, bool* pHas) {
            cJSON* curItem = cur();
            if (getObject(sz)) {
                internal::serializeWrapper(*this, value);
                if (pHas) *pHas = true;
            }
            cur(curItem);
        }

        template<typename T>
        void decodeValue(const char* sz, std::vector<T>& value, bool* pHas) {
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
        }
        template<typename K, typename V>
        void decodeValue(const char* sz, std::map<K, V>& value, bool* pHas) {
            cJSON* curItem = cur();
            if (getObject(sz)) {
                int32_t size = getMapSize();
                if (size&& !value.empty())
                    value.clear();
                for (int32_t i = 0; i < size; ++i) {
                    cJSON* lastItem = cur();
                    std::string key = getChildName(i);
                    V item = V();
                    decodeValue(key.c_str(), item, NULL);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                    cur(lastItem);
                }
                if (pHas) *pHas = true;
            }
            cur(curItem);
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

        void decodeValue(const char* sz, bool& value, bool* pHas);
        void decodeValue(const char* sz, uint32_t& value, bool* pHas);
        void decodeValue(const char* sz, int32_t& value, bool* pHas);
        void decodeValue(const char* sz, uint64_t& value, bool* pHas);
        void decodeValue(const char* sz, int64_t& value, bool* pHas);
        void decodeValue(const char* sz, float& value, bool* pHas);
        void decodeValue(const char* sz, double& value, bool* pHas);
        void decodeValue(const char* sz, std::string& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<bool>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<uint32_t>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<int32_t>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<uint64_t>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<int64_t>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<float>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<double>& value, bool* pHas);
        void decodeValue(const char* sz, std::vector<std::string>& value, bool* pHas);

        CJSONDecoder& operator >>(std::vector<int32_t>& value);
        CJSONDecoder& operator >>(std::vector<float>& value);
        CJSONDecoder& operator >>(std::vector<double>& value);
        CJSONDecoder& operator >>(std::vector<std::string>& value);

        bool getObject(const char* sz);
        int32_t getArraySize()const;
        void getArrayItem(int32_t i);
        int32_t getMapSize() const;
        const char* getChildName(int32_t i)const;
        void cur(cJSON* item) { _cur = item; }
        cJSON* cur() { return _cur; }
    };

}

#endif