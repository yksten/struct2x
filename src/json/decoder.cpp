#include "json/decoder.h"


namespace serialize {

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

    JSONDecoder::JSONDecoder(const char* sz, uint32_t length)
        :_str(sz, length), _mgr(NULL) {
    }

    void JSONDecoder::decodeValue(const char* sz, bool& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<bool>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, uint32_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<uint32_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, int32_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<int32_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, uint64_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<uint64_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, int64_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<int64_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, float& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<float>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, double& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<double>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::string& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<std::string>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<bool>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<bool>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<uint32_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<uint32_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<int32_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<int32_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<uint64_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<uint64_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<int64_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<int64_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<float>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<float>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<double>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<double>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<std::string>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<std::string>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::convertValue(bool& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(int32_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(uint32_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(int64_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(uint64_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(float& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(double& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(std::string& value, const char* cValue, uint32_t length, bool* pHas) {
//        value.append(cValue, length);
        bool ret = parse_string(value, cValue, length);
        if (pHas) *pHas = ret;
    }

    void JSONDecoder::convertArray(std::vector<bool>& value, const char* cValue, uint32_t length, bool* pHas) {
        bool v = false;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<int32_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        int32_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<uint32_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        uint32_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<int64_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        int64_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<uint64_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        uint64_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<float>& value, const char* cValue, uint32_t length, bool* pHas) {
        float v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<double>& value, const char* cValue, uint32_t length, bool* pHas) {
        double v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<std::string>& value, const char* cValue, uint32_t length, bool* pHas) {
        value.push_back(std::string(cValue, length));
        if (pHas) *pHas = true;
    }

}
