#include <struct2x/json/GenericWriter.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define APPENDSTRING(str, szValue)                                                         \
str.append(1, '\"');                                                                       \
for (const char *ptr = szValue; *ptr; ++ptr) {                                             \
    if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\') {                        \
        str.append(1, *ptr);                                                               \
    } else {                                                                               \
        str.append(1, '\\');                                                               \
        switch (*ptr) {                                                                    \
            case '\\':      str.append(1, '\\');     break;                                \
            case '\"':      str.append(1, '\"');     break;                                \
            case '\b':      str.append(1, 'b');      break;                                \
            case '\f':      str.append(1, 'f');      break;                                \
            case '\n':      str.append(1, 'n');      break;                                \
            case '\r':      str.append(1, 'r');      break;                                \
            case '\t':      str.append(1, 't');      break;                                \
            default: {                                                                     \
                size_t nSize = str.size();                                                 \
                str.resize(nSize + 5, '\0');                                               \
                sprintf(&str.at(nSize), "u%04x", *ptr);                                    \
                break;                                                                     \
            }                                                                              \
        }                                                                                  \
    }                                                                                      \
} str.append(1, '\"');

namespace custom {

    void GenericWriter::Bool(bool b) {
        value_type &vt = _stack.back();
        if (vt.first == kkeyType) {
            colon(_str);
        } else if (vt.first == kValueType) {
            comma(_str);
            tab(_str, _stack.size());
        } else {
            tab(_str, _stack.size());
        }
        if (b) {
            _str.append("true");
        } else {
            _str.append("false");
        }
        vt.first = kValueType;
    }

    void GenericWriter::Int64(int64_t i64) {
        value_type &vt = _stack.back();
        if (vt.first == kkeyType) {
            colon(_str);
        } else if (vt.first == kValueType) {
            comma(_str);
            tab(_str, _stack.size());
        } else {
            tab(_str, _stack.size());
        }
        char buffer[32] = {0};
        snprintf(buffer, 32, "%lld", i64);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Uint64(uint64_t u64) {
        value_type &vt = _stack.back();
        if (vt.first == kkeyType) {
            colon(_str);
        } else if (vt.first == kValueType) {
            comma(_str);
            tab(_str, _stack.size());
        } else {
            tab(_str, _stack.size());
        }
        char buffer[32] = {0};
        snprintf(buffer, 32, "%llu", u64);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Double(double d) {
        value_type &vt = _stack.back();
        if (vt.first == kkeyType) {
            colon(_str);
        } else if (vt.first == kValueType) {
            comma(_str);
            tab(_str, _stack.size());
        } else {
            tab(_str, _stack.size());
        }
        char buffer[64] = {0};
        snprintf(buffer, 64, "%0.8f", d);
        _str.append(buffer);
    }

    GenericWriter &GenericWriter::Key(const char *szKey) {
        if (szKey) {
            value_type &vt = _stack.back();
            if (vt.second) { comma(_str); }
            tab(_str, _stack.size());
            APPENDSTRING(_str, szKey);
            vt.second++;
            vt.first = kkeyType;
        }
        return *this;
    }

    void GenericWriter::String(const char *szValue) {
        value_type &vt = _stack.back();
        if (vt.first == kkeyType) {
            colon(_str);
        } else if (vt.first == kValueType) {
            comma(_str);
            tab(_str, _stack.size());
        } else {
            tab(_str, _stack.size());
        }
        APPENDSTRING(_str, szValue);
        vt.first = kValueType;
    }

    void GenericWriter::StartObject() {
        if (!_stack.empty()) {
            if (_stack.back().first == kkeyType) {
                colon(_str);
            } else if (_stack.back().first == kValueType) {
                comma(_str);
            } else {
                tab(_str, _stack.size());
            }
        }
        _stack.push_back(value_type(kNullType, 0));
        leftBrace(_str);
    }

    void GenericWriter::EndObject() {
        rightBrace(_str, _stack.size());
        _stack.pop_back();
    }

    void GenericWriter::StartArray() {
        if (!_stack.empty()) {
            if (_stack.back().first == kkeyType) {
                colon(_str);
            } else if (_stack.back().first == kValueType) {
                comma(_str);
            } else {
                tab(_str, _stack.size());
            }
        }
        leftBracket(_str);
        _stack.push_back(value_type(kNullType, 0));
    }

    void GenericWriter::EndArray() {
        rightBracket(_str, _stack.size());
        _stack.pop_back();
    }

    void GenericWriter::colon(std::string& str) const {
        str.append(1, ':');
        if (_formatted) {
            str.append(1, ' ');
        }
    }

    void GenericWriter::comma(std::string& str) const {
        str.append(1, ',');
        if (_formatted) {
            str.append(1, '\n');
        }
    }

    void GenericWriter::leftBrace(std::string& str) const {
        str.append(1, '{');
        if (_formatted) {
            str.append(1, '\n');
        }
    }

    void GenericWriter::rightBrace(std::string& str, int32_t layer) const {
        if (_formatted) {
            str.append(1, '\n');
            tab(str, layer - 1);
        }
        str.append(1, '}');
    }

    void GenericWriter::leftBracket(std::string& str) const {
        str.append(1, '[');
        if (_formatted) {
            str.append(1, '\n');
        }
    }

    void GenericWriter::rightBracket(std::string& str, int32_t layer) const {
        if (_formatted) {
            str.append(1, '\n');
            tab(str, layer - 1);
        }
        str.append(1, ']');
    }

    void GenericWriter::tab(std::string& str, int32_t layer) const {
        if (_formatted) {
            for (int32_t idx = 0; idx < layer; ++idx) {
                str.append(1, '\t');
            }
        }
    }

} // namespace custom
