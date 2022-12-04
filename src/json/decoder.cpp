#include <struct2x/json/decoder.h>


namespace struct2x {

    static unsigned parse_hex4(const char *str) {
        unsigned h=0;
        if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
        h=h<<4;str++;
        if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
        h=h<<4;str++;
        if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
        h=h<<4;str++;
        if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
        return h;
    }

    /* Parse the input text into an unescaped cstring, and populate item. */
    static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
    static bool parse_string(std::string& strValue, const char* str, uint32_t length) {
        if (!length) { return false; }    /* not a string! */
        const char *ptr = str; unsigned uc, uc2;
        for (uint32_t i = 0; i < length;++i) {
            if (ptr[i]!='\\') {
                strValue.append(1, ptr[i]);
            } else {
                ++i;
                switch (ptr[i]) {
                    case 'b': strValue.append(1, '\b');    break;
                    case 'f': strValue.append(1, '\f');    break;
                    case 'n': strValue.append(1, '\n');    break;
                    case 'r': strValue.append(1, '\r');    break;
                    case 't': strValue.append(1, '\t');    break;
                    case 'u':     /* transcode utf16 to utf8. */
                        uc=parse_hex4(ptr+i+1);i+=4;    /* get the unicode char. */
                        if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)    break;    /* check for invalid.    */
                        /* UTF16 surrogate pairs.    */
                        if (uc>=0xD800 && uc<=0xDBFF) {
                            if (ptr[i+1]!='\\' || ptr[i+2]!='u')    break;    /* missing second-half of surrogate.    */
                            uc2=parse_hex4(ptr+i+3);i+=6;
                            if (uc2<0xDC00 || uc2>0xDFFF)        break;    /* invalid second-half of surrogate.    */
                            uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                        }

                        if (uc<0x80) {
                            strValue.append(1, uc | firstByteMark[1]);
                        } else if (uc<0x800) {
                            strValue.append(1, '\0');
                            strValue.append(1, (uc | 0x80) & 0xBF);
                            uc >>= 6;
                        } else if (uc<0x10000) {
                            strValue.append(2, '\0');
                            strValue.append(1, (uc | 0x80) & 0xBF);
                            uc >>= 6;
                        } else {
                            strValue.append(3, '\0');
                            strValue.append(1, (uc | 0x80) & 0xBF);
                            uc >>= 6;
                        }
                        break;
                    default:  strValue.append(1, ptr[i]); break;
                }
            }
        }
        return true;
    }


    JSONDecoder::JSONDecoder(const char* str, bool caseInsensitive) : _convertByType(true), _caseInsensitive(caseInsensitive), _cur(custom::GenericReader(_vec).Parse(str)) {
        assert(_cur);
    }

    JSONDecoder::~JSONDecoder() {
    }

    JSONDecoder& JSONDecoder::setConvertByType(bool convertByType) {
        _convertByType = convertByType;
        return *this;
    }

    void JSONDecoder::decodeValue(const char* name, bool& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_BOOL)) {
            value = item2Bool(*item);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, uint32_t& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_NUMBER)) {
            value = (uint32_t)custom::GenericReader::convertUint(item->value, item->valueSize);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, int32_t& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_NUMBER)) {
            value = (int32_t)custom::GenericReader::convertInt(item->value, item->valueSize);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, uint64_t& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_NUMBER)) {
            value = custom::GenericReader::convertUint(item->value, item->valueSize);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, int64_t& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_NUMBER)) {
            value = custom::GenericReader::convertInt(item->value, item->valueSize);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, float& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_NUMBER)) {
            value = (float)custom::GenericReader::convertDouble(item->value, item->valueSize);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, double& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_NUMBER)) {
            value = custom::GenericReader::convertDouble(item->value, item->valueSize);
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* name, std::string& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_STRING) && item->value && item->valueSize) {
            value.clear();
            bool result = parse_string(value, item->value, item->valueSize);
            if (pHas) *pHas = true;
            assert(result);
        }
    }

    void JSONDecoder::decodeValue(const char* name, std::vector<bool>& value, bool* pHas) {
        const custom::GenericValue* item = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
        if (item && checkItemType(*item, custom::GenericValue::VALUE_ARRAY)) {
            for (const custom::GenericValue* child = item->child; child && checkItemType(*item, custom::GenericValue::VALUE_BOOL); child = child->next) {
                value.push_back(item2Bool(*child));
                if (pHas) *pHas = true;
            }
        }
    }

    bool JSONDecoder::checkItemType(const custom::GenericValue& item, const int type) const {
        if (!_convertByType) {
            return true;
        }
        return (item.type == type);
    }

    bool JSONDecoder::item2Bool(const custom::GenericValue& item) const {
        if (item.type == custom::GenericValue::VALUE_BOOL) {
            return (item.valueSize == 4);
        } else if (!_convertByType) {
            if (item.type == custom::GenericValue::VALUE_NUMBER) {
                int value = custom::GenericReader::convertInt(item.value, item.valueSize);
                return (value != 0);
            } else if (item.type == custom::GenericValue::VALUE_STRING) {
                std::string value;
                if (parse_string(value, item.value, item.valueSize)) {
                    return (atoi(value.c_str()));
                }
            } else {
                ;
            }
        }
        return false;
    }

}
