#include <struct2x/json/GenericReader.h>
#include <assert.h>
#include <string>


namespace custom {

    class StringStream {
        const char* _src;
    public:
        typedef const char value_type;
        explicit StringStream(const char* src) : _src(src) {}
        value_type Peek() const { if (isEnd()) return '\0'; return *_src; }
        value_type Second2Last() const { return *(_src - 1); }
        value_type Take() { return *_src++; }
        value_type* Strart() const { return _src; }
        bool isEnd() const {return (*_src == '\0'); }
    };

/*------------------------------------------------------------------------------*/

    GenericReader::GenericReader(std::vector<GenericValue>& vec) : _cur(NULL), _alloc(vec) {
    }

    GenericReader::~GenericReader() {
    }

    int64_t GenericReader::convertInt(const char* value, uint32_t length) {
        if (!length) return 0;
        
        int64_t result = 0;
        bool bMinus = false;
        if (value[0] == '-') {
            bMinus = true;
        }
        for (uint32_t idx = bMinus; idx < length; ++idx) {
            result *= 10;
            result += value[idx] - '0';
        }
        if (bMinus) result = 0 - result;
        return result;
    }

    uint64_t GenericReader::convertUint(const char* value, uint32_t length) {
        if (!length) return 0;
        
        uint64_t result = 0;
        for (uint32_t idx = 0; idx < length; ++idx) {
            result *= 10;
            result += value[idx] - '0';
        }
        return result;
    }

    static inline double decimal(uint8_t n, uint32_t num) {
        double db = n * 1.0f;
        while (num--)
            db = db / 10;
        return db;
    }

    double GenericReader::convertDouble(const char* value, uint32_t length) {
        if (!length) return 0.0f;
        
        double result = 0.0;
        for (uint32_t idx = 0, bFlag = false, num = 0; idx < length; ++idx) {
            const char& c = value[idx];
            if (c == '.') {
                bFlag = true;
                continue;
            }
            uint8_t n = c - '0';
            if (!bFlag) {
                result *= 10;
                result += n;
            } else {
                ++num;
                result += decimal(n, num);
            }
        }
        return result;
    }

    uint32_t GenericReader::GetObjectSize(const GenericValue* parent) {
       uint32_t size = 0;
        if (parent) {
            for (const GenericValue* child = parent->child; child; child = child->next) {
                ++size;
            }
        }
       return size;
    }

    static int32_t strcasecmp(const char *s1, const char *s2) {
        if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
        for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)    if(*s1 == 0)    return 0;
        return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
    }

    const GenericValue* GenericReader::GetObjectItem(const GenericValue* parent, const char* name, bool caseInsensitive) {
        if (!parent || !name) {
            return parent;
        }
        
        for (GenericValue* child = parent->child; child; child = child->next) {
            if (child->key && (strlen(name) == child->keySize)) {
                if (!caseInsensitive) {
                    if (strncmp(name, child->key, child->keySize) == 0) {
                        return child;
                    }
                } else {
                    if (strcasecmp(name, std::string(child->key, child->keySize).c_str()) == 0) {
                        return child;
                    }
                }
            }
        }

        return NULL;
    }

    const GenericValue* GenericReader::Parse(const char* src) {
        do {
            ++_alloc;
            StringStream is(src);
            if (is.Peek() != '\0') {
                ParseValue(is);
            }
        } while(false);
        
        //
        _alloc.reSize();
        
        GenericValue* root = _alloc.allocValue();
        _cur = root;
        
        StringStream is(src);
        if (is.Peek() != '\0') {
            ParseValue(is);
        }
        return root;
    }

    void GenericReader::ParseValue(StringStream& is) {
        switch (is.Peek()) {
            case 'n': ParseNull(is); break;
            case 't': ParseTrue(is); break;
            case 'f': ParseFalse(is); break;
            case '"': ParseString(is); break;
            case '{': { GenericValue* parent = _cur; setItemType(GenericValue::VALUE_OBJECT); ParseObject(is); _cur = parent; } break;
            case '[': { GenericValue* parent = _cur; setItemType(GenericValue::VALUE_ARRAY); ParseArray(is); _cur = parent; } break;
            default:
                ParseNumber(is);
                break;
        }
        if (_strError[0])
            return;
    }

    void GenericReader::ParseKey(StringStream& is) {
        assert(is.Peek() == '\"');
        is.Take();  // Skip '\"'
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '\"') {
                setItemKey(szStart, is.Strart() - szStart);
                is.Take();  // Skip '\"'
                return;
            }
        }
        setError("KeyInvalid");
    }

    void GenericReader::ParseNull(StringStream& is) {
        assert(is.Peek() == 'n');
        is.Take();

        if (Consume(is, 'u') && Consume(is, 'l') && Consume(is, 'l')) {
            setItemValue(GenericValue::VALUE_NULL, "null", 4);
        } else {
            setError("ValueInvalid");
        }
    }

    void GenericReader::ParseTrue(StringStream& is) {
        assert(is.Peek() == 't');
        const char* szStart = is.Strart();
        is.Take();

        if (Consume(is, 'r') && Consume(is, 'u') && Consume(is, 'e')) {
            setItemValue(GenericValue::VALUE_BOOL, szStart, is.Strart() - szStart);
        } else {
            setError("ValueInvalid");
        }
    }

    void GenericReader::ParseFalse(StringStream& is) {
        assert(is.Peek() == 'f');
        const char* szStart = is.Strart();
        is.Take();

        if (Consume(is, 'a') && Consume(is, 'l') && Consume(is, 's') && Consume(is, 'e')) {
            setItemValue(GenericValue::VALUE_BOOL, szStart, is.Strart() - szStart);
        } else {
            setError("ValueInvalid");
        }
    }

    void GenericReader::ParseString(StringStream& is) {
        assert(is.Peek() == '\"');
        is.Take();  // Skip '\"'
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '\"' && is.Second2Last() != '\\') {
                setItemValue(GenericValue::VALUE_STRING, szStart, is.Strart() - szStart);
                is.Take();  // Skip '\"'
                return;
            }
        }
        setError("ValueInvalid");
    }

    void GenericReader::ParseNumber(StringStream& is) {
        const char* szStart = is.Strart();
        
        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '-' || is.Peek() == '.' || (is.Peek() >= '0' && is.Peek() <= '9')) {
                continue;
            } else {
                setItemValue(GenericValue::VALUE_NUMBER, szStart, is.Strart() - szStart);
                return;
            }
        }
        setError("ValueInvalid");
    }

    void GenericReader::ParseArray(StringStream& is) {
        assert(is.Peek() == '[');
        is.Take();  // Skip '['

        SkipWhitespace(is);
        if (Consume(is, ']')) {
            return;
        }

        for (uint32_t elementIndex = 0;;) {
            getChildItem(elementIndex);
            ParseValue(is);
            ++elementIndex;
            SkipWhitespace(is);

            if (Consume(is, ',')) {
                SkipWhitespace(is);
            } else if (Consume(is, ']')) {
                return;
            } else {
                setError("ParseErrorArrayMissCommaOrSquareBracket");
                return;
            }
        }
        setError("ValueArrayInvalid");
    }

    void GenericReader::ParseObject(StringStream& is) {
        assert(is.Peek() == '{');
//        const char* szStart = is.Strart();
        is.Take();  // Skip '{'

        SkipWhitespace(is);
        if (is.Peek() == '}') {
            is.Take();
            return;
        }

        for (uint32_t elementIndex = 0; !is.isEnd();) {
            if (is.Peek() != '"') {
                setError("ObjectMissName");
                return;
            }
            getChildItem(elementIndex);
            ParseKey(is);

            SkipWhitespace(is);
            if (!Consume(is, ':')) {
                setError("ObjectMissColon");
                return;
            }

            SkipWhitespace(is);
            ParseValue(is);
            ++elementIndex;

            SkipWhitespace(is);
            switch (is.Peek()) {
                case ',':
                    is.Take();
                    SkipWhitespace(is);
                    break;
                case '}':
                    is.Take();
                    return;
                default:
                    setError("ObjectMissCommaOrCurlyBracket");
                    break; // This useless break is only for making warning and coverage happy
            }
        }
    }

    void GenericReader::setItemType(const int32_t type) {
        if (_cur) {
            assert(GenericValue::VALUE_NULL <= type && type <= GenericValue::VALUE_OBJECT);
            _cur->type = type;
        }
    }

    void GenericReader::getChildItem(const uint32_t elementIndex) {
        if (_cur) {
            GenericValue* temp = _cur;
            _cur = _alloc.allocValue();
            _cur->prev = temp;
            if (!elementIndex) {
                temp->child = _cur;
            } else {
                temp->next = _cur;
            }
        } else {
            ++_alloc;
        }
    }

    void GenericReader::setItemKey(const char* key, const uint32_t keySize) {
        if (_cur) {
            assert(key);
            assert(keySize);
            _cur->key = key;
            _cur->keySize = keySize;
        }
    }

    void GenericReader::setItemValue(const int32_t type, const char* value, const uint32_t valueSize) {
        if (_cur) {
            assert(GenericValue::VALUE_NULL <= type && type <= GenericValue::VALUE_OBJECT);
            assert(value);
            assert(valueSize);
            _cur->type = type;
            _cur->value = value;
            _cur->valueSize = valueSize;
        }
    }

    bool GenericReader::Consume(StringStream& is, const char expect) {
        if (is.Peek() == expect) {
            is.Take();
            return true;
        } else
            return false;
    }

    void GenericReader::SkipWhitespace(StringStream& is) {
        for (;;) {
            const char c = is.Peek();
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                is.Take();
            } else {
                break;
            }
        }
    }

}
