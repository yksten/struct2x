#ifndef __RAPIDJSON_DECODER_H__
#define __RAPIDJSON_DECODER_H__
#include "struct2x.h"
#include <string>
#include <map>
#include "thirdParty/rapidjson/reader.h"


namespace struct2x {

    class converter {
    public:
        void convert(void*, const void*, bool*);
        typedef void(*convert_t)(void*, const void*, bool*);

        converter(convert_t func, void* value, bool* pHas) :_func(func), _value(value), _pHas(pHas) {}

        void operator()(const void* cValue) const {
            (*_func)(_value, cValue, _pHas);
        }
    private:
        convert_t _func;
        void* _value;
        bool* _pHas;
    };

    template<typename P>
    inline converter bind(void(*f)(P&, const P&, bool*), P& value, bool* pHas) {
        return converter(converter::convert_t(f), &value, pHas);
    }

    class EXPORTAPI convertHandler {
        const converter* _converter;
        const std::map<std::string, converter>& _map;
    public:
        convertHandler(const std::map<std::string, converter>& map) :_converter(NULL), _map(map) {}

        bool Null();
        bool Bool(bool b);
        bool Int(int i);
        bool Uint(unsigned u);
        bool Int64(int64_t i);
        bool Uint64(uint64_t u);
        bool Double(double d);
        bool RawNumber(const char* str, unsigned length, bool copy);
        bool String(const char* str, unsigned length, bool copy);
        bool StartObject();
        bool Key(const char* str, unsigned length, bool copy);
        bool EndObject(unsigned memberCount);
        bool StartArray();
        bool EndArray(unsigned elementCount);
    };

    class EXPORTAPI rapidjsonDecoder {
        std::map<std::string, converter> _map;
        rapidjson::StringStream _str;
    public:
        rapidjsonDecoder(const char* sz) :_str(sz) {}
        ~rapidjsonDecoder() {}

        template<typename T>
        bool operator >> (T& value) {
            internal::serializeWrapper(*this, value);
            convertHandler handler(_map);
            rapidjson::Reader reader;
            reader.Parse(_str, handler);
            return true;
        }

        template<typename T>
        rapidjsonDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, value.value);
        }

        template<typename T>
        rapidjsonDecoder& convert(const char* sz, T& value, bool* pHas = NULL) {
            cJSON* curItem = cur();
            if (getObject(sz)) {
                internal::serializeWrapper(*this, value);
                if (pHas) *pHas = true;
            }
            cur(curItem);
            return *this;
        }

        template<typename T>
        rapidjsonDecoder& convert(const char* sz, std::vector<T>& value, bool* pHas = NULL) {
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
        rapidjsonDecoder& convert(const char* sz, std::map<K, V>& value, bool* pHas = NULL) {
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

        rapidjsonDecoder& convert(const char* sz, bool& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, uint32_t& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, int32_t& value, bool* pHas = NULL) {
            _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
            return *this;
        }
        rapidjsonDecoder& convert(const char* sz, uint64_t& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, int64_t& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, float& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, double& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::string& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<bool>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<uint32_t>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<int32_t>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<uint64_t>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<int64_t>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<float>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<double>& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, std::vector<std::string>& value, bool* pHas = NULL);

        template<typename P>
        static void convertValue(P& value, const P& cValue, bool* pHas) {
            value = cValue;
            if (pHas)
                *pHas = true;
        }
    };
}


#endif