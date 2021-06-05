#include "GenericReader.h"
#include <assert.h>

namespace custom {

    resetStruct::resetStruct(uint8_t** pStruct, uint8_t* pTarget)
        :_pStruct(pStruct), _pTarget(pTarget) {
    }
    void resetStruct::reset() {
        *_pStruct = _pTarget;
    }

    /*------------------------------------------------------------------------------*/

    jsonConverter::jsonConverter(convert_t func, size_t value, size_t has, obtainConvert_t obtain, clear_t _clearArray)
        :_szKey(NULL), _keyLength(0), _func(func), _value(value), _has(has), _obtain(obtain), _clearArray(_clearArray), _last(NULL), _lastStruct(NULL, NULL) {
    }

    void jsonConverter::operator()(uint8_t* pStruct, const char* cValue, uint32_t length) const {
        (*_func)((pStruct + _value), cValue, length, (bool*)(_has ? pStruct + _has : NULL));
        if (_last) {
            jsonConverter* temp = const_cast<jsonConverter*>(this);
            _lastStruct.reset();
            *temp = *_last;
        }
    }

    void* jsonConverter::getConvert(uint8_t* pStruct, void* owner) const {
        if (_obtain)
            return (*_obtain)(pStruct + _value, _szKey, _keyLength, owner);
        return NULL;
    }

    void jsonConverter::clear(uint8_t* pStruct) const {
        if (_clearArray)
            (*_clearArray)(pStruct + _value);
    }

    void jsonConverter::setKey(const char* szKey, uint32_t length) const {
        _szKey = szKey;
        _keyLength = length;
    }

    void jsonConverter::setLast(const jsonConverter* last, const resetStruct& lastStruct) const {
        _last = const_cast<jsonConverter*>(last);
        _lastStruct = lastStruct;
    }

    /*------------------------------------------------------------------------------*/

    jsonConverterMgr::jsonConverterMgr(const void* pStruct, bool isMap)
        :_owner(NULL), _pStruct((uint8_t*)pStruct), _isMap(isMap) {
    }

    jsonConverterMgr::jsonConverterMgr(const jsonConverterMgr& that)
        : _owner(that._owner), _pStruct(NULL), _isMap(that._isMap), _map(that._map) {
    }

    const uint8_t* jsonConverterMgr::getStruct() const {
        return _pStruct;
    }

    bool jsonConverterMgr::isMap() const {
        return _isMap;
    }

    void jsonConverterMgr::clear() {
        _map.clear();
    }

    void jsonConverterMgr::insert(const std::pair<std::string, jsonConverter>& item) {
        _map.insert(item);
    }

    jsonConverterMgr::const_iterator jsonConverterMgr::end() const {
        return _map.end();
    }

    jsonConverterMgr::const_iterator jsonConverterMgr::find(const char* sz, uint32_t length) const {
        return _map.find(std::string(sz, length));
    }

    void jsonConverterMgr::setStruct(void* value, void* owner) {
        if (value && owner) {
            _owner = (Handler*)owner;
            _owner->_stackStruct.push_back(_owner->_struct);
            _owner->_struct = (uint8_t*)value;
        }
    }

    /*------------------------------------------------------------------------------*/

    StringStream::StringStream(Ch* src, uint32_t length) : _src(src), _length(length) {
    }

    StringStream::Ch StringStream::Peek() const {
        if (isEnd())
            return '\0';
        return *_src;
    }

    StringStream::Ch StringStream::Second2Last() const {
        return *(_src-1);
    }

    StringStream::Ch StringStream::Take() {
        --_length;
        return *_src++;
    }

    StringStream::Ch* StringStream::Strart() const {
        return _src;
    }

    bool StringStream::isEnd() const {
        return (_length == 0);
    }

    /*------------------------------------------------------------------------------*/

    Handler::Handler(jsonConverterMgr* mgr, void* pStruct) :_converter(NULL), _mgr(mgr), _struct((uint8_t*)pStruct) {
        assert(_mgr);
        _mgr->setStruct(pStruct, this);
    }

    bool Handler::Key(const char* sz, uint32_t length) {
        assert(_mgr);
        if (_mgr->isMap() && _converter) {
            const jsonConverter* tempConverter = _converter;
            uint8_t* tempStruct = _struct;
            _converter->setKey(sz, length);

            jsonConverterMgr* mgr = (jsonConverterMgr*)_converter->getConvert(_struct, this);
            _converter = &mgr->find(sz, length)->second;
            _converter->setLast(tempConverter, resetStruct(&_struct, tempStruct));
        } else {
            jsonConverterMgr::const_iterator it = _mgr->find(sz, length);
            if (it != _mgr->end()) {
                _converter = &it->second;
            } else {
                _converter = NULL;
            }
        }
        return true;
    }

    bool Handler::Value(const char* sz, uint32_t length) {
        if (_converter) {
            (*_converter)(_struct, sz, length);
        }
        return true;
    }

    bool Handler::StartObject() {
        if (_converter) {
            _stackFunction.push_back(_converter);

            assert(_mgr);
            _stackMgr.push_back(_mgr);
            _mgr = (jsonConverterMgr*)_converter->getConvert(_struct, this);
        }
        return true;
    }

    bool Handler::EndObject(uint32_t memberCount) {
        if (!_stackMgr.empty()) {
            assert(_mgr);
            bool bIsMap = _mgr->isMap();
            _mgr = _stackMgr.back();
            _stackMgr.erase(_stackMgr.begin() + _stackMgr.size() - 1);

            _converter = _stackFunction.back();
            _stackFunction.erase(_stackFunction.begin() + _stackFunction.size() - 1);

            if (!bIsMap || memberCount) {
                _struct = _stackStruct.back();
                _stackStruct.erase(_stackStruct.begin() + _stackStruct.size() - 1);
            }
        }
        return true;
    }

    bool Handler::StartArray() {
        return true;
    }

    bool Handler::EndArray(uint32_t elementCount) {
        if (!elementCount && _converter) {
            _converter->clear(_struct);
        }
        return true;
    }

    void Handler::convert(bool& value, const char* sz, uint32_t length) {
        if (length == 4 && strncmp(sz, "true", 4) == 0) {
            value = true;
        } else if (length == 5 && strncmp(sz, "false", 5) == 0) {
            value = false;
        } else {
            assert(false);
        }
    }

    void Handler::convert(int32_t& value, const char* sz, uint32_t length) {
        if (!length) return;
        value = 0;
        bool bMinus = false;
        if (sz[0] == '-') {
            bMinus = true;
        }
        for (uint32_t idx = bMinus; idx < length; ++idx) {
            value *= 10;
            value += sz[idx] - '0';
        }
        if (bMinus) value = 0 - value;
    }

    void Handler::convert(uint32_t& value, const char* sz, uint32_t length) {
        if (!length) return;
        value = 0;
        for (uint32_t idx = 0; idx < length; ++idx) {
            value *= 10;
            value += sz[idx] - '0';
        }
    }

    void Handler::convert(int64_t& value, const char* sz, uint32_t length) {
        if (!length) return;
        value = 0;
        bool bMinus = false;
        if (sz[0] == '-') {
            bMinus = true;
        }
        for (uint32_t idx = bMinus; idx < length; ++idx) {
            value *= 10;
            value += sz[idx] - '0';
        }
        if (bMinus) value = 0 - value;
    }

    void Handler::convert(uint64_t& value, const char* sz, uint32_t length) {
        if (!length) return;
        value = 0;
        for (uint32_t idx = 0; idx < length; ++idx) {
            value *= 10;
            value += sz[idx] - '0';
        }
    }

    static inline double decimal(uint8_t n, uint32_t num) {
        double db = n * 1.0f;
        while (num--)
            db = db / 10;
        return db;
    }

    void Handler::convert(float& value, const char* sz, uint32_t length) {
        if (!length) return;
        value = 0;
        bool bMinus = false;
        if (sz[0] == '-') {
            bMinus = true;
        }
        for (uint32_t idx = bMinus, bFlag = false, num = 0; idx < length; ++idx) {
            const char& c = sz[idx];
            if (c == '.') {
                bFlag = true;
                continue;
            }
            uint8_t n = c - '0';
            if (!bFlag) {
                value *= 10;
                value += n;
            } else {
                ++num;
                value += decimal(n, num);
            }
        }
        if (bMinus) value = 0 - value;
    }

    void Handler::convert(double& value, const char* sz, uint32_t length) {
        if (!length) return;
        value = 0;
        for (uint32_t idx = 0, bFlag = false, num = 0; idx < length; ++idx) {
            const char& c = sz[idx];
            if (c == '.') {
                bFlag = true;
                continue;
            }
            uint8_t n = c - '0';
            if (!bFlag) {
                value *= 10;
                value += n;
            } else {
                ++num;
                value += decimal(n, num);
            }
        }
    }

    /*------------------------------------------------------------------------------*/
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

    CustomGenericReader::CustomGenericReader() :_result(true) {
    }

    bool CustomGenericReader::Parse(StringStream& is, Handler& handler) {
        if (is.Peek() != '\0') {
            ParseValue(is, handler);
        }
        return _result;
    }

    const char* CustomGenericReader::getError()const {
        return _strError.c_str();
    }
    void CustomGenericReader::setError(const char* sz) {
        _result = false;
        _strError.append(sz);
    }

    void CustomGenericReader::ParseValue(StringStream& is, Handler& handler) {
        switch (is.Peek()) {
            case 'n': ParseNull(is, handler); break;
            case 't': ParseTrue(is, handler); break;
            case 'f': ParseFalse(is, handler); break;
            case '"': ParseString(is, handler); break;
            case '{': ParseObject(is, handler); break;
            case '[': ParseArray(is, handler); break;
            default:
                ParseNumber(is, handler);
                break;
        }
        if (!_result)
            return;
    }

    void CustomGenericReader::ParseKey(StringStream& is, Handler& handler) {
        assert(is.Peek() == '\"');
        is.Take();  // Skip '\"'
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '\"') {
                handler.Key(szStart, is.Strart() - szStart);
                is.Take();  // Skip '\"'
                return;
            }
        }
        setError("KeyInvalid");
    }

    void CustomGenericReader::ParseNull(StringStream& is, Handler& handler) {
        assert(is.Peek() == 'n');
        is.Take();

        if (Consume(is, 'u') && Consume(is, 'l') && Consume(is, 'l')) {
            handler.Value("", 0);
        } else {
            setError("ValueInvalid");
        }
    }

    void CustomGenericReader::ParseTrue(StringStream& is, Handler& handler) {
        assert(is.Peek() == 't');
        const char* szStart = is.Strart();
        is.Take();

        if (Consume(is, 'r') && Consume(is, 'u') && Consume(is, 'e')) {
            handler.Value(szStart, is.Strart() - szStart);
        } else {
            setError("ValueInvalid");
        }
    }

    void CustomGenericReader::ParseFalse(StringStream& is, Handler& handler) {
        assert(is.Peek() == 'f');
        const char* szStart = is.Strart();
        is.Take();

        if (Consume(is, 'a') && Consume(is, 'l') && Consume(is, 's') && Consume(is, 'e')) {
            handler.Value(szStart, is.Strart() - szStart);
        } else {
            setError("ValueInvalid");
        }
    }

    void CustomGenericReader::ParseString(StringStream& is, Handler& handler) {
        assert(is.Peek() == '\"');
        is.Take();  // Skip '\"'
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '\"' && is.Second2Last() != '\\') {
                handler.Value(szStart, is.Strart() - szStart);
                is.Take();  // Skip '\"'
                return;
            }
        }
        setError("ValueInvalid");
    }

    void CustomGenericReader::ParseArray(StringStream& is, Handler& handler) {
        assert(is.Peek() == '[');
        is.Take();  // Skip '['

        if (!handler.StartArray()) {
            setError("ParseErrorTermination");
            return;
        }

        SkipWhitespace(is);
        if (Consume(is, ']')) {
            if (!handler.EndArray(0)) {
                setError("ParseErrorTermination");
            }
            return;
        }
        for (uint32_t elementCount = 0;;) {
            ParseValue(is, handler);
            ++elementCount;
            SkipWhitespace(is);

            if (Consume(is, ',')) {
                SkipWhitespace(is);
            } else if (Consume(is, ']')) {
                if ((!handler.EndArray(elementCount))) {
                    setError("ParseErrorTermination");
                }
                return;
            } else {
                setError("ParseErrorArrayMissCommaOrSquareBracket");
                return;
            }
        }
        setError("ValueArrayInvalid");
    }

    void CustomGenericReader::ParseNumber(StringStream& is, Handler& handler) {
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '-' || is.Peek() == '.' || (is.Peek() >= '0' && is.Peek() <= '9')) {
                continue;
            } else {
                handler.Value(szStart, is.Strart() - szStart);
                return;
            }
        }
        setError("ValueInvalid");
    }

    void CustomGenericReader::ParseObject(StringStream& is, Handler& handler) {
        assert(is.Peek() == '{');
        const char* szStart = is.Strart();
        is.Take();  // Skip '{'

        if (!handler.StartObject()) {
            setError("ParseErrorTermination");
            return;
        }

        SkipWhitespace(is);
        if (is.Peek() == '}') {
            is.Take();
            if (!handler.EndObject(0)) {
                setError("ParseErrorTermination");
            }
            return;
        }

        for (uint32_t memberCount = 0; !is.isEnd();) {
            if (is.Peek() != '"') {
                setError("ObjectMissName");
                return;
            }
            ParseKey(is, handler);

            SkipWhitespace(is);
            if (!Consume(is, ':')) {
                setError("ObjectMissColon");
                return;
            }

            SkipWhitespace(is);
            ParseValue(is, handler);
            ++memberCount;

            SkipWhitespace(is);
            switch (is.Peek()) {
                case ',':
                    is.Take();
                    SkipWhitespace(is);
                    break;
                case '}':
                    is.Take();
                    if (!handler.EndObject(memberCount)) {
                        setError("ParseErrorTermination");
                    }
                    return;
                default:
                    setError("ObjectMissCommaOrCurlyBracket");
                    break; // This useless break is only for making warning and coverage happy
            }
        }
    }

}
