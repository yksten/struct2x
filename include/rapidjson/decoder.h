#ifndef __RAPIDJSON_DECODER_H__
#define __RAPIDJSON_DECODER_H__
#include "serialize.h"
#include <string>
#include <map>
#include "thirdParty/rapidjson/reader.h"


namespace serialize {

    class rapidJsonConverter {
    public:
        typedef void(*convert_t)(void*, const void*, bool*);
        typedef void*(*obtainConvert_t)(void*, const char*);
        typedef void(*clear_t)(void*);

        rapidJsonConverter(convert_t func, size_t value, size_t has, obtainConvert_t obtain, clear_t _clearArray)
            :_szKey(NULL), _func(func), _value(value), _has(has), _obtain(obtain), _clearArray(_clearArray), _last(NULL) {}

        void operator()(uint8_t* pStruct, const void* cValue) const {
            (*_func)((pStruct + _value), cValue, (bool*)(_has ? pStruct + _has : NULL));
            if (_last) {
                rapidJsonConverter* temp = const_cast<rapidJsonConverter*>(this);
                *temp = *_last;
            }
        }

        void* getConvert(uint8_t* pStruct) const {
            if (_obtain)
                return (*_obtain)(pStruct + _value, _szKey);
            return NULL;
        }

        void clear(uint8_t* pStruct) const {
            if (_clearArray)
                (*_clearArray)(pStruct + _value);
        }

        void setKey(const char* szKey) const { _szKey = szKey; }
        const char* getKey() const { return _szKey; }
        void setLast(const rapidJsonConverter* last) const { _last = const_cast<rapidJsonConverter*>(last); }

    private:
        mutable const char* _szKey;
        convert_t _func;
        size_t _value;
        size_t _has;
        obtainConvert_t _obtain;
        clear_t _clearArray;
        mutable rapidJsonConverter* _last;
    };

    class rapidJsonConverterMgr {
        uint8_t* _pStruct;
        bool _isMap;
        std::map<std::string, rapidJsonConverter> _map;
    public:
        typedef std::map<std::string, rapidJsonConverter>::const_iterator const_iterator;
        explicit rapidJsonConverterMgr(void* pStruct, bool isMap = false) :_pStruct((uint8_t*)pStruct), _isMap(isMap) {}

        inline void setStruct(void* pStruct) { _pStruct = (uint8_t*)pStruct; }
        inline uint8_t* getStruct() const { return _pStruct; }
        inline bool isMap() const { return _isMap; }

        inline void clear() { _map.clear(); }

        inline void insert(const std::pair<std::string, rapidJsonConverter>& item) {
            _map.insert(item);
        }

        inline const_iterator end() const { return _map.end(); }

        inline const_iterator find(const char* sz) const {
            return _map.find(sz);
        }

        template<typename P>
        static inline rapidJsonConverter bind(void(*f)(P&, const P&, bool*), size_t value, size_t has, rapidJsonConverterMgr*(*obtain)(P&, const char*) = NULL, void(*clearArray)(P&) = NULL) {
            return rapidJsonConverter(rapidJsonConverter::convert_t(f), value, has, rapidJsonConverter::obtainConvert_t(obtain), rapidJsonConverter::clear_t(clearArray));
        }

        template<typename P>
        static inline rapidJsonConverter bindArray(void(*f)(std::vector<P>&, const P&, bool*), size_t value, size_t has, rapidJsonConverterMgr*(*obtain)(std::vector<P>&, const char*) = NULL, void(*clearArray)(std::vector<P>&) = NULL) {
            return rapidJsonConverter(rapidJsonConverter::convert_t(f), value, has, rapidJsonConverter::obtainConvert_t(obtain), rapidJsonConverter::clear_t(clearArray));
        }

        template<typename K, typename V>
        static inline rapidJsonConverter bindMap(void(*f)(V&, const V&, bool*), size_t value, size_t has, rapidJsonConverterMgr*(*obtain)(std::map<K, V>&, const char*) = NULL, void(*clearArray)(V&) = NULL) {
            return rapidJsonConverter(rapidJsonConverter::convert_t(f), value, has, rapidJsonConverter::obtainConvert_t(obtain), rapidJsonConverter::clear_t(clearArray));
        }
    };

    class EXPORTAPI convertHandler {
        const rapidJsonConverter* _converter;
        std::vector<const rapidJsonConverter*> _stackFunction;
        rapidJsonConverterMgr* _mgr;
        std::vector<rapidJsonConverterMgr*> _stack;
    public:
        explicit convertHandler(rapidJsonConverterMgr* mgr) :_converter(NULL), _mgr(mgr) {}

        bool Null();
        bool Bool(bool b);
        bool Int(int i);
        bool Uint(unsigned u);
        bool Int64(int64_t i);
        bool Uint64(uint64_t u);
        bool Double(double d);
        bool RawNumber(const char* str, unsigned length, bool copy);
        bool String(const char* str, unsigned length, bool copy);
        bool Key(const char* str, unsigned length, bool copy);
        bool StartObject();
        bool EndObject(unsigned memberCount);
        bool StartArray();
        bool EndArray(unsigned elementCount);
    };

    class EXPORTAPI rapidjsonDecoder {
        rapidJsonConverterMgr* _mgr;
        rapidjson::InsituStringStream _str;

        template<typename T>
        static rapidJsonConverterMgr getSet(T& value) {
            rapidJsonConverterMgr mgr(&value);
            rapidjsonDecoder decoder(NULL);
            decoder._mgr = &mgr;
            internal::serializeWrapper(decoder, value);
            return mgr;
        }
    public:
        rapidjsonDecoder(const char* sz) :_str(const_cast<char*>(sz)), _mgr(NULL) {}
        ~rapidjsonDecoder() {}

        template<typename T>
        static rapidJsonConverterMgr* obtain(T& value, const char*) {
            static rapidJsonConverterMgr mgr = getSet(value);
            mgr.setStruct(&value);
            return &mgr;
        }

        template<typename T>
        static rapidJsonConverterMgr* obtainArray(std::vector<T>& value, const char*) {
            value.push_back(T());
            rapidJsonConverterMgr* mgr = obtain(value.back(), NULL);
            return mgr;
        }

        template<typename K, typename V>
        static rapidJsonConverterMgr* obtain(std::map<K, V>& value, const char* sz) {
            static rapidJsonConverterMgr mgr(NULL, true);
            if (sz) {
                mgr.clear();
                K key = internal::STOT::type<typename internal::TypeTraits<K>::Type>::strto(sz);
                mgr.setStruct(&value[key]);
                mgr.insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindMap<K, V>(&rapidjsonDecoder::convertValue, 0, 0)));
            }
            return &mgr;
        }

        template<typename T>
        bool operator>>(T& value) {
            _mgr = obtain(value, NULL);
            _mgr->setStruct(&value);
            convertHandler handler(_mgr);
            return rapidjson::Reader().Parse<rapidjson::kParseInsituFlag>(_str, handler).IsError();
        }

        template<typename T>
        rapidjsonDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        rapidjsonDecoder& convert(const char* sz, T& value, bool* pHas = NULL) {
            size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
            size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
            _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<T>(&rapidjsonDecoder::convertValue, offset, has, &rapidjsonDecoder::obtain)));
            return *this;
        }

        template<typename T>
        static void clearArray(std::vector<T>& value) {
            if (!value.empty()) {
                value.clear();
            }
        }

        template<typename T>
        rapidjsonDecoder& convert(const char* sz, std::vector<T>& value, bool* pHas = NULL) {
            size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
            size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
            _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<T>(&rapidjsonDecoder::convertArray, offset, has, &rapidjsonDecoder::obtainArray, &rapidjsonDecoder::clearArray)));
            return *this;
        }

        template<typename K, typename V>
        rapidjsonDecoder& convert(const char* sz, std::map<K, V>& value, bool* pHas = NULL) {
            size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
            size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
            _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindMap<K, V>(&rapidjsonDecoder::convertValue, offset, has, &rapidjsonDecoder::obtain)));
            return *this;
        }

        rapidjsonDecoder& convert(const char* sz, bool& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, uint32_t& value, bool* pHas = NULL);
        rapidjsonDecoder& convert(const char* sz, int32_t& value, bool* pHas = NULL);
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
            if (pHas) *pHas = true;
        }

        template<typename P>
        static void convertArray(std::vector<P>& value, const P& cValue, bool* pHas) {
            value.push_back(cValue);
            if (pHas) *pHas = true;
        }

        template<typename K, typename V>
        static void convertMap(std::map<K, V>& value, const V& cValue, bool* pHas) {
            //value.push_back(cValue);
            if (pHas) *pHas = true;
        }
    };

}


#endif