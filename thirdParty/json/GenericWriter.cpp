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
        Stack::value_type &vt = _stack.top();
        if (vt.first == kkeyType) { _str.append(1, ':'); } else if (vt.first == kValueType) { _str.append(1, ','); }
        if (b) {
            _str.append("true");
        } else {
            _str.append("false");
        }
        vt.first = kValueType;
    }

    void GenericWriter::Int64(int64_t i64) {
        Stack::value_type &vt = _stack.top();
        if (vt.first == kkeyType) { _str.append(1, ':'); } else if (vt.first == kValueType) { _str.append(1, ','); }
        char buffer[32] = {0};
        snprintf(buffer, 32, "%lld", i64);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Uint64(uint64_t u64) {
        Stack::value_type &vt = _stack.top();
        if (vt.first == kkeyType) { _str.append(1, ':'); } else if (vt.first == kValueType) { _str.append(1, ','); }
        char buffer[32] = {0};
        snprintf(buffer, 32, "%llu", u64);
        _str.append(buffer);
        vt.first = kValueType;
    }

    void GenericWriter::Double(double d) {
        Stack::value_type &vt = _stack.top();
        if (vt.first == kkeyType) { _str.append(1, ':'); } else if (vt.first == kValueType) { _str.append(1, ','); }
        char buffer[64] = {0};
        snprintf(buffer, 64, "%0.8f", d);
        _str.append(buffer);
    }

    GenericWriter &GenericWriter::Key(const char *szKey) {
        if (szKey) {
            Stack::value_type &vt = _stack.top();
            if (vt.second) { _str.append(1, ','); }
            APPENDSTRING(_str, szKey);
            vt.second++;
            vt.first = kkeyType;
        }
        return *this;
    }

    void GenericWriter::String(const char *szValue) {
        Stack::value_type &vt = _stack.top();
        if (vt.first == kkeyType) { _str.append(1, ':'); } else if (vt.first == kValueType) { _str.append(1, ','); }
        APPENDSTRING(_str, szValue);
        vt.first = kValueType;
    }

} // namespace custom
