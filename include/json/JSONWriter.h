#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__
#include <string>
#include <map>
#include "struct2x.h"


struct cJSON;
namespace struct2x {

    class EXPORTAPI JSONWriter {
        cJSON* _root;
        cJSON* _cur;
        JSONWriter();
        JSONWriter(const JSONWriter&);
        JSONWriter& operator=(const JSONWriter&);
    public:
        JSONWriter(const char* sz);
        ~JSONWriter();

        template<typename T>
        bool operator >> (T& value) {
            if (_cur) {
                internal::serializeWrapper(*this, value);
            }
            return (_cur) ? true : false;
        }

        template<typename T>
        JSONWriter& operator&(serializeItem<T> value) {
            return convert(value.name(), value.value());
        }

        template<typename T>
        JSONWriter& convert(const char* sz, T& value) {
            cJSON* curItem = cur();
            getObject(sz);
            internal::serializeWrapper(*this, value);
            cur(curItem);
            return *this;
        }

        template<typename T>
        JSONWriter& convert(const char* sz, std::vector<T>& value) {
            cJSON* curItem = cur();
            getObject(sz);
            int size = getArraySize();
            if (size&& !value.empty())
                value.clear();
            for (int i = 0; i < size; ++i) {
                cJSON* lastItem = cur();
                getArrayItem(i);
                typename internal::TypeTraits<T>::Type item;
                this->operator>>(item);
                value.push_back(item);
                cur(lastItem);
            }
            cur(curItem);
            return *this;
        }
        template<typename K, typename V>
        JSONWriter& convert(const char* sz, std::map<K, V>& value) {
            cJSON* curItem = cur();
            getObject(sz);
            int size = getMapSize();
            if (size&& !value.empty())
                value.clear();
            for (int i = 0; i < size; ++i) {
                cJSON* lastItem = cur();
                std::string key;
                V item = V();
                getkeyValue(i, key, item);
                value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                cur(lastItem);
            }
            cur(curItem);
            return *this;
        }

        template<typename T>
        bool operator >>(std::vector<T>& value) {
            if (_cur) {
                int size = getArraySize();
                if (size && !value.empty())
                    value.clear();
                for (int i = 0; i < size; ++i) {
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
                int size = getMapSize();
                if (size && !value.empty())
                    value.clear();
                for (int i = 0; i < size; ++i) {
                    std::string key;
                    V item = V();
                    getkeyValue(i, key, item);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                }
            }
            return (_cur) ? true : false;
        }
        JSONWriter& convert(const char* sz, int& value);
        JSONWriter& convert(const char* sz, float& value);
        JSONWriter& convert(const char* sz, double& value);
        JSONWriter& convert(const char* sz, unsigned int& value);
        JSONWriter& convert(const char* sz, std::string& value);
        JSONWriter& convert(const char* sz, bool& value);
        JSONWriter& convert(const char* sz, std::vector<int>& value);
        JSONWriter& convert(const char* sz, std::vector<float>& value);
        JSONWriter& convert(const char* sz, std::vector<double>& value);
        JSONWriter& convert(const char* sz, std::vector<std::string>& value);
    private:
        JSONWriter& operator >>(std::vector<int>& value);
        JSONWriter& operator >>(std::vector<float>& value);
        JSONWriter& operator >>(std::vector<double>& value);
        JSONWriter& operator >>(std::vector<std::string>& value);

        void getObject(const char* sz);
        int getArraySize()const;
        void getArrayItem(int i);
        int getMapSize() const;
        const char* getChildName(int i)const;
        template<typename T>
        void getkeyValue(int i, std::string& key, T& value) {
            key = getChildName(i);
            convert(key.c_str(), value);
        }
        void cur(cJSON* item) { _cur = item; }
        cJSON* cur() { return _cur; }
    };

}

#endif