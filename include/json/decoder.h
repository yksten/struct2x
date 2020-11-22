#ifndef __JSON_DECODER_H__
#define __JSON_DECODER_H__
#include "struct2x.h"
#include <string>
#include <map>


namespace struct2x {

    struct converter {
        typedef void(*function_type)(void*, const char*, uint32_t, bool*);
        converter(function_type func, void* value, bool* pHas) :_func(func), _value(value), _pHas(pHas) {}
        void operator()(const char* cValue, uint32_t length) const {
            (*_func)(_value, cValue, length, _pHas);
        }
        template<typename T>
        static converter bind(void(*f)(T&, const char*, uint32_t, bool*), T& value, bool* pHas) {
            return converter(function_type(f), &value, pHas);
        }

        template<typename T>
        static converter bind(void(*f)(std::vector<T>&, const char*, uint32_t, bool*), std::vector<T>& value, bool* pHas) {
            return converter(function_type(f), &value, pHas);
        }
    private:
        function_type _func;
        void* _value;
        bool* _pHas;
    };

    class StringStream {
        typedef const char Ch;
        Ch* _src;
        uint32_t _length;
    public:
        StringStream(Ch* src, uint32_t length) : _src(src),_length(length) {}
        // Read
        Ch Peek() { if (isEnd()) return '\0';  return *_src; }
        Ch Take() { --_length; return *_src++; }
        Ch* Strart() const { return _src; }
        bool isEnd() const { return (_length == 0); }
    };

    class EXPORTAPI BaseHandler {
    public:
        virtual ~BaseHandler() {}
        virtual bool Key(const char* sz, unsigned length) = 0;
        virtual bool Value(const char* sz, unsigned length) = 0;
    };

    typedef std::pair<const char*, converter> function_value;
    class EXPORTAPI Handler :public BaseHandler {
    protected:
        const converter* _converter;
        const std::vector<function_value>* _set;
    public:
        Handler(const std::vector<function_value>& set);
        virtual bool Key(const char* sz, unsigned length);
        virtual bool Value(const char* sz, unsigned length);
    };

    class EXPORTAPI GenericReader {
        bool _result;
        std::string _strError;
    public:
        GenericReader();
        bool Parse(StringStream& is, BaseHandler& handler);
        const char* getError()const;
    private:
        void setError(const char* sz);
        void ParseValue(StringStream& is, BaseHandler& handler, bool bSkipObj);
        void ParseKey(StringStream& is, BaseHandler& handler);
        void ParseNull(StringStream& is, BaseHandler& handler);
        void ParseTrue(StringStream& is, BaseHandler& handler);
        void ParseFalse(StringStream& is, BaseHandler& handler);
        void ParseString(StringStream& is, BaseHandler& handler);
        void ParseNumber(StringStream& is, BaseHandler& handler);
        void ParseArray(StringStream& is, BaseHandler& handler);
        void ParseObject(StringStream& is, BaseHandler& handler);
        void ParseObjectAsStr(StringStream& is, BaseHandler& handler);
    };

    class EXPORTAPI JSONDecoder {
        StringStream _str;
        std::vector<function_value> _set;
    public:
        JSONDecoder(const char* sz, uint32_t length) :_str(sz, length) {}
        ~JSONDecoder() {}

        template<typename T>
        bool operator >> (T& value) {
            internal::serializeWrapper(*this, value);
            Handler handler(_set);
            return GenericReader().Parse(_str, handler);
        }

        template<typename T>
        JSONDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, value.value);
        }

        template<typename T>
        JSONDecoder& convert(const char* sz, T& value, bool* pHas = NULL) {
            _set.push_back(function_value(sz, converter::bind(&JSONDecoder::convertValue, value, pHas)));
            return *this;
        }

        template<typename T>
        JSONDecoder& convert(const char* sz, std::vector<T>& value, bool* pHas = NULL) {
            _set.push_back(function_value(sz, converter::bind(&JSONDecoder::convertArray, value, pHas)));
            return *this;
        }
        template<typename K, typename V>
        JSONDecoder& convert(const char* sz, std::map<K, V>& value, bool* pHas = NULL) {
            _set.push_back(function_value(sz, converter::bind(&JSONDecoder::convertMap, value, pHas)));
            return *this;
        }
    private:
        static void convertValue(bool& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(int32_t& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(uint32_t& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(int64_t& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(uint64_t& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(float& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(double& value, const char* sz, uint32_t length, bool* pHas);
        static void convertValue(std::string& value, const char* sz, uint32_t length, bool* pHas);

        static void convertArray(std::vector<bool>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<int32_t>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<uint32_t>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<int64_t>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<uint64_t>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<float>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<double>& value, const char* sz, uint32_t length, bool* pHas);
        static void convertArray(std::vector<std::string>& value, const char* sz, uint32_t length, bool* pHas);

        template<typename T>
        static void convertValue(T& value, const char* cValue, uint32_t length, bool* pHas) {
            if (length) {
                JSONDecoder decoder(cValue.first, cValue.second);
                if (!decoder.operator>>(value))
                    return;
                if (pHas) *pHas = true;
            }
        }

        template<typename T>
        static void convertArray(std::vector<T>& value, const char* cValue, uint32_t length, bool* pHas) {
            if (length) {
                value.clear();
                std::vector<const char*> stack;
                for (uint32_t idx = 0, bFlag = true; idx < length; ++idx) {
                    const char c = cValue[idx];
                    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                        continue;
                    }
                    if (c == '{') {
                        stack.push_back(cValue + idx);
                    }
                    else if (c == '}') {
                        T temp = T();
                        const char* szBin = stack[stack.size() - 1];
                        JSONDecoder decoder(szBin, (cValue + idx) - szBin);
                        if (decoder.operator>>(temp))
                            value.push_back(temp);
                        stack.erase(stack.begin() + stack.size() - 1);
                    }
                }
                if (pHas) *pHas = true;
            }
        }

        template<typename K, typename V>
        class MapHandler : public BaseHandler {
            K _key;
            V _value;
            std::map<K, V>& _map;
        public:
            MapHandler(std::map<K, V>& map) :_map(map) {}
            bool Key(const char* sz, unsigned length) {
                convertValue(_key, sz, length, NULL);
                return true;
            }
            bool Value(const char* sz, unsigned length) {
                convertValue(_value, sz, length, NULL);
                _map.insert(std::pair<K, V>(_key, _value));
                return true;
            }
        };

        template<typename K, typename V>
        static void convertMap(std::map<K, V>& value, const char* cValue, uint32_t length, bool* pHas) {
            if (length) {
                StringStream str(cValue, length);
                MapHandler<K, V> handler(value);
                GenericReader().Parse(str, handler);

                if (pHas) *pHas = true;
            }
        }
    };

}


#endif