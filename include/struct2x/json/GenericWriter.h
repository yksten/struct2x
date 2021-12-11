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
        
        explicit Stack(uint32_t capacity) :_top(0),_base(NULL),_stacksize(0) {
            _base = (value_type*)malloc(capacity * sizeof(value_type));
            _stacksize = capacity;
        }

        bool empty() const { return (_top == 0); }
        value_type& top() { return _base[_top-1];}
        const value_type& top() const { return _base[_top-1]; }
        void pop() { if (_top) { --_top; } }
        uint32_t layer() const { return _top; }

        void push(const value_type& val) {
            if(_top == _stacksize) {
                _base = (value_type*)realloc(_base,(_stacksize+1)*sizeof(value_type));
                if(!_base) return;
                _stacksize++;
            }
            _base[_top++] = val;
        }
        
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
        bool _formatted;
    public:
        explicit GenericWriter(std::string& str, bool formatted = false) : _stack(32), _str(str), _formatted(formatted) {}
        void Bool(bool b);
        void Int64(int64_t i64);
        void Uint64(uint64_t u64);
        void Double(double d);
        GenericWriter& Key(const char* szKey);
        void String(const char* szValue);
        void StartObject();
        void EndObject();
        void StartArray();
        void EndArray();

        void Separation() {
            if (!_stack.empty() && _stack.top().first == kNullType) {
                comma(_str);
            }
        }

        bool result() const { return _stack.empty();}
        // :
        void colon(std::string& str) const;
        // ,
        void comma(std::string& str) const;
        // {
        void leftBrace(std::string& str) const;
        // }
        void rightBrace(std::string& str, int32_t layer) const;
        // [
        void leftBracket(std::string& str) const;
        // ]
        void rightBracket(std::string& str, int32_t layer) const;
        // \t
        void tab(std::string& str, int32_t layer) const;
    };

}

#endif
