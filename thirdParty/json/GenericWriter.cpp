#include "GenericWriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef FASTJSON_UINT64_C2
#define FASTJSON_UINT64_C2(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#endif

namespace custom {

    sqstack::sqstack(uint32_t capacity) :_top(0),_base(NULL),_stacksize(0) {
        _base = (value_type*)malloc(capacity * sizeof(value_type));
        _stacksize = capacity;
    }

    bool sqstack::empty() const {
        if(_top == 0) {
            return true;
        }
        return false;
    }

    sqstack::value_type& sqstack::top() {
        return _base[_top-1];
    }

    const sqstack::value_type& sqstack::top() const {
        return _base[_top-1];
    }

    void sqstack::pop() {
        if (_top) {
            --_top;
        }
    }

    void sqstack::push(const value_type& val) {
        if(_top == _stacksize) {
            _base = (value_type*)realloc(_base,(_stacksize+1)*sizeof(value_type));
            if(!_base) return 0;
            _stacksize++;
        }
        _base[_top++] = val;
    }

/*----------------------------------------------------------------------------------------------------*/

    GenericWriter::GenericWriter(std::string& str) :_str(str),_stack(32) {
    }

    void GenericWriter::Null() {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        _str.append("null");
        vt.first = kValueType;
    }

    void GenericWriter::Bool(bool b) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        if (b) {
            _str.append("true");
        } else {
            _str.append("false");
        }
        vt.first = kValueType;
    }

    void GenericWriter::Int(int32_t i) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        char buffer[24] = { 0 };
        snprintf(buffer, 24, "%d", i);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Uint(uint32_t u) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        char buffer[24] = { 0 };
        snprintf(buffer, 24, "%u", u);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Int64(int64_t i64) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        char buffer[32] = { 0 };
        snprintf(buffer, 32, "%lld", i64);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Uint64(uint64_t u64) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        char buffer[32] = { 0 };
        snprintf(buffer, 32, "%llu", u64);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Double(double d) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        char buffer[32] = { 0 };
        snprintf(buffer, 32, "%lf", d);
        _str.append(buffer);
    }

    void GenericWriter::Key(const char* szKey) {
        if (!szKey) {
            return;
        }
        sqstack::value_type& vt = _stack.top();
        if (vt.second) {
            _str.append(1, ',');
        }
        _str.append(1, '"').append(szKey).append(1, '\"');
        vt.second++;
        vt.first = kkeyType;
    }

    void GenericWriter::String(const char* szValue) {
        sqstack::value_type& vt = _stack.top();
        if (vt.first == kNullType) {
            ;
        } else if (vt.first == kkeyType) {
            _str.append(1, ':');
        } else if (vt.first == kValueType) {
            _str.append(1, ',');
        }
        _str.append(1, '\"');
        for (const char* ptr = szValue; *ptr; ++ptr) {
            if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') {
                _str.append(1, *ptr);
            } else {
                _str.append(1, '\\');
                switch (*ptr) {
                    case '\\':    _str.append(1, '\\');    break;
                    case '\"':    _str.append(1, '\"');    break;
                    case '\b':    _str.append(1, 'b');    break;
                    case '\f':    _str.append(1, 'f');    break;
                    case '\n':    _str.append(1, 'n');    break;
                    case '\r':    _str.append(1, 'r');    break;
                    case '\t':    _str.append(1, 't');    break;
                    default: {
                        size_t nSize = _str.size();
                        _str.resize(nSize + 5, '\0');
                        snprintf(&_str.at(nSize), 5, "u%04x", *ptr);
                        break;
                    }
                }
            }
        }
        _str.append(1, '\"');
        vt.first = kValueType;
    }

    void GenericWriter::StartObject() {
        if (!_stack.empty() && _stack.top().first == kkeyType) {
            _str.append(1, ':');
        }
        _stack.push(sqstack::value_type(kNullType, 0));
        _str.append(1, '{');
    }

    void GenericWriter::EndObject() {
        _str.append(1, '}');
        _stack.pop();
    }

    void GenericWriter::StartArray() {
        if (!_stack.empty()) {
            _str.append(1, ':');
        }
        _str.append(1, '[');
        _stack.push(sqstack::value_type(kNullType, 0));
    }

    void GenericWriter::EndArray() {
        _str.append(1, ']');
        _stack.pop();
    }

    void GenericWriter::Separation() {
        _str.append(1, ',');
    }

    bool GenericWriter::result() const {
        return _stack.empty();
    }

}
