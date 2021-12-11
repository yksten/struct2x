#ifndef __JSON_DECODER_H__
#define __JSON_DECODER_H__

#include <string>
#include <map>
#include <struct2x/traits.h>
#include <struct2x/json/GenericReader.h>


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

    class JSONDecoder {
        bool _gnoreItemType;                // check field type
        bool _CaseInsensitive;              // key case insensitive
        custom::GenericReader _reader;
        const custom::GenericValue* _cur;
    public:
        FORCEINLINE JSONDecoder(const char* str, bool gnoreType = false, bool caseInsensitive = false) : _gnoreItemType(gnoreType), _CaseInsensitive(caseInsensitive), _cur(_reader.Parse(str)) {
            assert(_cur);
        }

        template<typename T>
        FORCEINLINE bool operator>>(T& value) {
            if (!_cur) return false;
            const custom::GenericValue* parent = _cur;
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            return (parent == _cur);
        }
        
        template<typename K, typename V>
        FORCEINLINE bool operator>>(std::map<K, V>& value) {
            if (!_cur) return false;
            const custom::GenericValue* parent = _cur;
            if (_cur) {
                value.clear();

                const custom::GenericValue* parentTemp = _cur;
                for (const custom::GenericValue* child = parent->child; child; child = child->next) {
                    std::string key(child->key, child->keySize);
                    V item = V();
                    decodeValue(key.c_str(), item, NULL);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                }
                _cur = parentTemp;
            }
            return (parent == _cur);
        }

        template<typename T>
        FORCEINLINE JSONDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        FORCEINLINE JSONDecoder& convert(const char* name, T& value, bool* pHas = NULL) {
            decodeValue(name, *(typename internal::TypeTraits<T>::Type*)(&value), pHas);
            return *this;
        }
    private:
        template<typename T>
        FORCEINLINE void decodeValue(const char* name, T& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (_cur) {
                internal::serializeWrapper(*this, value);
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }

        template<typename T>
        FORCEINLINE void decodeValue(const char* name, std::vector<T>& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (_cur) {
                uint32_t size = custom::GetObjectSize(_cur);
                if (size) {
                    value.resize(size);
                }
                const custom::GenericValue* parentTemp = _cur;
                _cur = _cur->child;
                for (uint32_t idx = 0; _cur && (idx < size); (_cur = _cur->next) && ++idx) {
                    decodeValue(NULL, *(typename internal::TypeTraits<T>::Type*)(&value.at(idx)), NULL);
                }
                _cur = parentTemp;
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }

        template<typename K, typename V>
        FORCEINLINE void decodeValue(const char* name, std::map<K, V>& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (_cur) {
                value.clear();

                const custom::GenericValue* parentTemp = _cur;
                for (const custom::GenericValue* child = _cur->child; child; child = child->next) {
                    std::string key(child->key, child->keySize);
                    V item = V();
                    decodeValue(key.c_str(), item, NULL);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                }
                _cur = parentTemp;
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }
        
        FORCEINLINE void decodeValue(const char* name, bool& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_BOOL)) {
                value = item2Bool(*item);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, uint32_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_NUMBER)) {
                value = (uint32_t)custom::GenericReader::convertUint(item->value, item->valueSize);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, int32_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_NUMBER)) {
                value = (int32_t)custom::GenericReader::convertInt(item->value, item->valueSize);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, uint64_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_NUMBER)) {
                value = custom::GenericReader::convertUint(item->value, item->valueSize);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, int64_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_NUMBER)) {
                value = custom::GenericReader::convertInt(item->value, item->valueSize);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, float& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_NUMBER)) {
                value = (float)custom::GenericReader::convertDouble(item->value, item->valueSize);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, double& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_NUMBER)) {
                value = custom::GenericReader::convertDouble(item->value, item->valueSize);
                if (pHas) *pHas = true;
            }
        }

        FORCEINLINE void decodeValue(const char* name, std::string& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_STRING) && item->value && item->valueSize) {
                value.clear();
                bool result = parse_string(value, item->value, item->valueSize);
                if (pHas) *pHas = true;
                assert(result);
            }
        }

        FORCEINLINE void decodeValue(const char* name, std::vector<bool>& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name, _CaseInsensitive);
            if (item && checkItemType(*item, custom::VALUE_ARRAY)) {
                for (const custom::GenericValue* child = item->child; child && checkItemType(*item, custom::VALUE_BOOL); child = child->next) {
                    value.push_back(item2Bool(*child));
                    if (pHas) *pHas = true;
                }
            }
        }
        
        inline bool checkItemType(const custom::GenericValue& item, const int type) const {
            if (_gnoreItemType) {
                return true;
            }
            return (item.type == type);
        }
        
        inline bool item2Bool(const custom::GenericValue& item) const {
            if (item.type == custom::VALUE_BOOL) {
                return (item.valueSize == 4);
            } else if (_gnoreItemType) {
                if (item.type == custom::VALUE_NUMBER) {
                    int value = custom::GenericReader::convertInt(item.value, item.valueSize);
                    return (value != 0);
                } else if (item.type == custom::VALUE_STRING) {
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
        
    };

}


#endif
