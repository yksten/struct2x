#ifndef __GENERIC_READER_H__
#define __GENERIC_READER_H__

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <cstdlib>
#include <string>
#include <cassert>
#include <vector>


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


    struct GenericValue {
        enum { VALUE_NULL = 0, VALUE_BOOL = 1, VALUE_NUMBER = 2, VALUE_STRING = 3, VALUE_ARRAY = 4, VALUE_OBJECT = 5 };
        
        int32_t type;
        
        const char* key;
        uint32_t keySize;
        
        const char* value;
        uint32_t valueSize;
        
        struct GenericValue* prev;
        struct GenericValue* next;
        struct GenericValue* child;
        
        GenericValue() :type(VALUE_NULL), key(NULL), keySize(0), value(NULL), valueSize(0), prev(NULL), next(NULL), child(NULL) {}
    };


    class StringStream;

    class EXPORTAPI GenericReader {
        class GenericValueAllocator {
            uint32_t _curIndex;
            uint32_t _capacity;
            std::vector<GenericValue>& _vec;
        public:
            explicit GenericValueAllocator(std::vector<GenericValue>& vec) : _curIndex(0), _capacity(0), _vec(vec) {}
            void operator++() { ++_capacity; }
            void reSize() { assert(_vec.empty()); _vec.resize(_capacity); }
            GenericValue* allocValue() { assert(_curIndex < _vec.size()); return &_vec.at(_curIndex++); }
        };
        GenericValue* _cur;
        GenericValueAllocator _alloc;
        std::string _strError;
    public:
        explicit GenericReader(std::vector<GenericValue>& vec);
        ~GenericReader();
        const GenericValue* Parse(const char* src);
        const char* getError() const { return _strError.c_str(); }
        
        static int64_t convertInt(const char* value, uint32_t length);
        static uint64_t convertUint(const char* value, uint32_t length);
        static double convertDouble(const char* value, uint32_t length);
        static uint32_t GetObjectSize(const GenericValue* parent);
        static const GenericValue* GetObjectItem(const GenericValue* parent, const char* name, bool caseInsensitive);
    private:
        void setError(const char* sz) { _strError = sz; }
        void ParseValue(StringStream& is);
        void ParseKey(StringStream& is);
        void ParseNull(StringStream& is);
        void ParseTrue(StringStream& is);
        void ParseFalse(StringStream& is);
        void ParseString(StringStream& is);
        void ParseNumber(StringStream& is);
        void ParseArray(StringStream& is);
        void ParseObject(StringStream& is);
        
        void setItemType(const int32_t type);
        void getChildItem(const uint32_t elementIndex);
        void setItemKey(const char* key, const uint32_t keySize);
        void setItemValue(const int32_t type, const char* value, const uint32_t valueSize);
        
        static bool Consume(StringStream& is, const char expect);
        static void SkipWhitespace(StringStream& is);
        
    };

}

#endif
