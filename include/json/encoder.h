#ifndef __FASTJSON_ENCODER_H__
#define __FASTJSON_ENCODER_H__
#include "struct2x.h"
#include <string>
#include <map>
#include <stack>


namespace struct2x {

    class EXPORTAPI GenericWriter {
        enum Type {
            kNullType = 0,      //!< null
            kkeyType = 1,       //!< key
            kValueType = 2,     //!< value
        };
        typedef std::pair<Type, uint32_t> value_type;
        std::stack<value_type> _stack;
        std::string _str;
    public:
        void Null();
        void Bool(bool b);
        void Int(int32_t i);
        void Uint(uint32_t u);
        void Int64(int64_t i64);
        void Uint64(uint64_t u64);
        void Double(double d);
        void Key(const char* szKey);
        void String(const char* szValue);
        void StartObject();
        void EndObject();
        void StartArray();
        void EndArray();
        bool toString(std::string& str);
    };

    class EXPORTAPI JSONEncoder {
        GenericWriter _writer;
    public:
        JSONEncoder();
        ~JSONEncoder();

        bool toString(std::string& str);

        template<typename T>
        JSONEncoder& operator&(serializeItem<T> value) {
            return convert(value.name, value.value);
        }

        template<typename T>
        JSONEncoder& operator << (const T& value) {
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
        JSONEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            StartObject(sz);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            EndObject();
            return *this;
        }

        template<typename T>
        JSONEncoder& convert(const char* sz, const std::vector<T>& value, bool* pHas = NULL) {
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
        JSONEncoder& convert(const char* sz, const std::map<K, V>& value, bool* pHas = NULL) {
            StartObject(sz);
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            EndObject();
            return *this;
        }

        template<typename K, typename V>
        JSONEncoder& operator <<(const std::map<K, V>& value) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                const V& item = it->second;
                convert(internal::STOT::type<K>::tostr(it->first), item);
            }
            return *this;
        }

        JSONEncoder& convert(const char* sz, bool value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, uint32_t value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, int32_t value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, uint64_t value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, int64_t value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, float value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, double value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::string& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<bool>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<uint32_t>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<int32_t>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<uint64_t>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<int64_t>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<float>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<double>& value, bool* pHas = NULL);
        JSONEncoder& convert(const char* sz, const std::vector<std::string>& value, bool* pHas = NULL);
    private:
        void StartObject(const char* sz);
        void EndObject();
        void StartArray(const char* sz);
        void EndArray();
    };

}


#endif