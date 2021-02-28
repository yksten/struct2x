#ifndef __GENERIC_READER_H__
#define __GENERIC_READER_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>

#ifdef _MSC_VER
#ifdef EXPORTAPI 
#define EXPORTAPI _declspec(dllimport)
#else 
#define EXPORTAPI _declspec(dllexport)
#endif
#else
#define EXPORTAPI
#endif

namespace custom {

    class EXPORTAPI resetStruct {
        uint8_t** _pStruct;
        uint8_t* _pTarget;
    public:
        resetStruct(uint8_t** pStruct, uint8_t* pTarget);
        void reset();
    };

    class EXPORTAPI jsonConverter {
    public:
        typedef void(*convert_t)(void*, const char*, uint32_t, bool*);
        typedef void*(*obtainConvert_t)(void*, const char*, uint32_t, void*);
        typedef void(*clear_t)(void*);

        jsonConverter(convert_t func, size_t value, size_t has, obtainConvert_t obtain, clear_t _clearArray);
        void operator()(uint8_t* pStruct, const char* cValue, uint32_t length) const;
        void* getConvert(uint8_t* pStruct, void* owner) const;
        void clear(uint8_t* pStruct) const;
        void setKey(const char* szKey, uint32_t length) const;
        void setLast(const jsonConverter* last, const resetStruct& lastStruct) const;
    private:
        mutable const char* _szKey;
        mutable uint32_t _keyLength;
        convert_t _func;
        size_t _value;
        size_t _has;
        obtainConvert_t _obtain;
        clear_t _clearArray;
        mutable jsonConverter* _last;
        mutable resetStruct _lastStruct;
    };

    class Handler;
    class EXPORTAPI jsonConverterMgr {
        Handler* _owner;
        const uint8_t* _pStruct;
        bool _isMap;
        std::map<std::string, jsonConverter> _map;
    public:
        typedef std::map<std::string, jsonConverter>::const_iterator const_iterator;
        explicit jsonConverterMgr(const void* pStruct, bool isMap = false);
        jsonConverterMgr(const jsonConverterMgr& that);
        inline const uint8_t* getStruct() const;
        inline bool isMap() const;
        inline void clear();
        inline void insert(const std::pair<std::string, jsonConverter>& item);
        inline const_iterator end() const;
        inline const_iterator find(const char* sz, uint32_t length) const;
        void setStruct(void* value, void* owner);

        template<typename P>
        static inline jsonConverter bind(void(*f)(P&, const char*, uint32_t, bool*), size_t value, size_t has, jsonConverterMgr*(*obtain)(P&, const char*, uint32_t, void*) = NULL, void(*clearArray)(P&) = NULL) {
            return jsonConverter(jsonConverter::convert_t(f), value, has, jsonConverter::obtainConvert_t(obtain), jsonConverter::clear_t(clearArray));
        }

        template<typename P>
        static inline jsonConverter bindArray(void(*f)(std::vector<P>&, const char*, uint32_t, bool*), size_t value, size_t has, jsonConverterMgr*(*obtain)(std::vector<P>&, const char*, uint32_t, void*) = NULL, void(*clearArray)(std::vector<P>&) = NULL) {
            return jsonConverter(jsonConverter::convert_t(f), value, has, jsonConverter::obtainConvert_t(obtain), jsonConverter::clear_t(clearArray));
        }

        template<typename K, typename V>
        static inline jsonConverter bindMap(void(*f)(V&, const char*, uint32_t, bool*), size_t value, size_t has, jsonConverterMgr*(*obtain)(std::map<K, V>&, const char*, uint32_t, void*) = NULL, void(*clearArray)(V&) = NULL) {
            return jsonConverter(jsonConverter::convert_t(f), value, has, jsonConverter::obtainConvert_t(obtain), jsonConverter::clear_t(clearArray));
        }
    };

    class EXPORTAPI StringStream {
    public:
        typedef const char Ch;
        StringStream(Ch* src, uint32_t length);
        Ch Peek() const;
        Ch Take();
        Ch* Strart() const;
        bool isEnd() const;
    private:
        Ch* _src;
        Ch* _end;
        uint32_t _length;
    };

    class EXPORTAPI Handler {
        friend class jsonConverterMgr;
        const jsonConverter* _converter;
        std::vector<const jsonConverter*> _stackFunction;
        jsonConverterMgr* _mgr;
        std::vector<jsonConverterMgr*> _stackMgr;
        uint8_t* _struct;
        std::vector<uint8_t*> _stackStruct;
    public:
        Handler(jsonConverterMgr* mgr, void* pStruct);
        bool Key(const char* sz, uint32_t length);
        bool Value(const char* sz, uint32_t length);
        bool StartObject();
        bool EndObject(uint32_t memberCount);
        bool StartArray();
        bool EndArray(uint32_t elementCount);

        static void convert(bool& value, const char* sz, uint32_t length);
        static void convert(int32_t& value, const char* sz, uint32_t length);
        static void convert(uint32_t& value, const char* sz, uint32_t length);
        static void convert(int64_t& value, const char* sz, uint32_t length);
        static void convert(uint64_t& value, const char* sz, uint32_t length);
        static void convert(float& value, const char* sz, uint32_t length);
        static void convert(double& value, const char* sz, uint32_t length);
    };

    class EXPORTAPI CustomGenericReader {
        bool _result;
        std::string _strError;
    public:
        CustomGenericReader();
        bool Parse(StringStream& is, Handler& handler);
        const char* getError()const;
    private:
        void setError(const char* sz);
        void ParseValue(StringStream& is, Handler& handler);
        void ParseKey(StringStream& is, Handler& handler);
        void ParseNull(StringStream& is, Handler& handler);
        void ParseTrue(StringStream& is, Handler& handler);
        void ParseFalse(StringStream& is, Handler& handler);
        void ParseString(StringStream& is, Handler& handler);
        void ParseNumber(StringStream& is, Handler& handler);
        void ParseArray(StringStream& is, Handler& handler);
        void ParseObject(StringStream& is, Handler& handler);
    };

}

#endif