#ifndef __GENERIC_READER_H__
#define __GENERIC_READER_H__

#include <stdio.h>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <cassert>


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

    enum { VALUE_NULL = 0, VALUE_BOOL, VALUE_NUMBER, VALUE_STRING, VALUE_ARRAY, VALUE_OBJECT };

    typedef struct GenericValue {
        int32_t type;
        
        const char* key;
        uint32_t keySize;
        
        const char* value;
        uint32_t valueSize;
        
        struct GenericValue* prev;
        struct GenericValue* next;
        struct GenericValue* child;
    } GenericValue;

    inline const GenericValue* GetObjectItem(const GenericValue* parent, const char* name) {
        if (!name) {
            return parent;
        }
        
        if (parent) {
            for (GenericValue* child = parent->child; child; child = child->next) {
                if (child->key && (strlen(name) == child->keySize) && strncmp(name, child->key, child->keySize) == 0) {
                    return child;
                }
            }
        }
        return NULL;
    }

    inline uint32_t GetArraySize(const GenericValue* parent) {
        uint32_t size = 0;
        if (parent && parent->type == VALUE_ARRAY) {
            for (const GenericValue* child = parent->child; child; child = child->next) {
                ++size;
            }
        }
        return size;
    }

    inline uint32_t GetObjectSize(const GenericValue* parent) {
        uint32_t size = 0;
        if (parent && parent->type == VALUE_OBJECT) {
            for (const GenericValue* child = parent->child; child; child = child->next) {
                ++size;
            }
        }
        return size;
    }

    class MemoryPoolAllocator;

    class EXPORTAPI GenericReader {
        class StringStream {
            const char* _src;
            uint32_t _length;
        public:
            typedef const char value_type;
            StringStream(const char* src, uint32_t length) : _src(src), _length(length) {}
            value_type Peek() const { if (isEnd()) return '\0'; return *_src; }
            value_type Second2Last() const { return *(_src - 1); }
            value_type Take() { --_length; return *_src++; }
            value_type* Strart() const { return _src; }
            bool isEnd() const { return (_length == 0); }
        };
        
        enum { DEFAULTCACPCITY = 32 };
        MemoryPoolAllocator* _alloc;
        GenericValue* _cur;
        char _strError[64];
    public:
        GenericReader();
        ~GenericReader();
        const GenericValue* Parse(const char* src, uint32_t length);
        const char* getError() const { return _strError; }
        
        static int64_t convertInt(const char* value, uint32_t length);
        static uint64_t convertUint(const char* value, uint32_t length);
        static double convertDouble(const char* value, uint32_t length);
    private:
        void setError(const char* sz) { memcpy(_strError, sz, 64); }
        void ParseValue(StringStream& is);
        void ParseKey(StringStream& is);
        void ParseNull(StringStream& is);
        void ParseTrue(StringStream& is);
        void ParseFalse(StringStream& is);
        void ParseString(StringStream& is);
        void ParseNumber(StringStream& is);
        void ParseArray(StringStream& is);
        void ParseObject(StringStream& is);

        static bool Consume(StringStream& is, const char expect) {
            if (is.Peek() == expect) {
                is.Take();
                return true;
            } else
                return false;
        }

        static void SkipWhitespace(StringStream& is) {
            for (;;) {
                const char c = is.Peek();
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                    is.Take();
                } else {
                    break;
                }
            }
        }
        
    };

}

#endif
