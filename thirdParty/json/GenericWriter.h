#ifndef __GENERIC_WRITER_H__
#define __GENERIC_WRITER_H__

#include <stdint.h>
#include <string>

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

    class EXPORTAPI Stack {
    public:
        struct value_type {
            value_type(int32_t f, uint32_t s) :first(f), second(s) {}
            int32_t first;
            uint32_t second;
        };
        explicit Stack(uint32_t capacity);
        bool empty() const;
        value_type& top();
        const value_type& top() const;
        void pop();
        void push (const value_type& val);
        
    private:
        uint32_t _top;
        value_type* _base;
        uint32_t _stacksize;
    };

    class EXPORTAPI GenericWriter {
        enum Type {
            kNullType = 0,      //!< null
            kkeyType = 1,       //!< key
            kValueType = 2,     //!< value
        };
        Stack _stack;
        std::string& _str;
    public:
        explicit GenericWriter(std::string& str);
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
        void Separation();
        bool result() const;
    };

}

#endif
