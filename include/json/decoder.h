#ifndef __JSON_DECODER_H__
#define __JSON_DECODER_H__

#include "serialize.h"
#include <string>
#include <map>

#include "json/GenericReader.h"


namespace serialize {

    class JSONDecoder {
        custom::CustomGenericReader _reader;
        const custom::GenericValue* _cur;
    public:
        FORCEINLINE JSONDecoder(const char* str, uint32_t length) : _cur(_reader.Parse(str, length)) {
            assert(_cur);
        }

        template<typename T>
        FORCEINLINE bool operator>>(T& value) {
            const custom::GenericValue* parent = _cur;
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            return (parent == _cur);
        }
        
        template<typename K, typename V>
        FORCEINLINE bool operator>>(std::map<K, V>& value) {
            const custom::GenericValue* parent = _cur;
            if (_cur) {
                value.clear();

                const custom::GenericValue* parent = _cur;
                for (const custom::GenericValue* child = parent->child; child; child = child->next) {
                    std::string key(child->key, child->keySize);
                    V item = V();
                    decodeValue(key.c_str(), item, NULL);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                }
                _cur = parent;
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
            _cur = custom::GetObjectItem(_cur, name);
            if (_cur) {
                internal::serializeWrapper(*this, value);
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }

        template<typename T>
        FORCEINLINE void decodeValue(const char* name, std::vector<T>& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GetObjectItem(_cur, name);
            if (_cur) {
                int32_t size = custom::GetArraySize(_cur);
                if (size) {
                    value.resize(size);
                }
                const custom::GenericValue* parent = _cur;
                _cur = parent->child;
                for (uint32_t idx = 0; _cur && (idx < size); (_cur = _cur->next) && ++idx) {
                    decodeValue(NULL, *(typename internal::TypeTraits<T>::Type*)(&value.at(idx)), NULL);
                }
                _cur = parent;
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }

        template<typename K, typename V>
        FORCEINLINE void decodeValue(const char* name, std::map<K, V>& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GetObjectItem(_cur, name);
            if (_cur) {
                value.clear();

                const custom::GenericValue* parent = _cur;
                for (const custom::GenericValue* child = parent->child; child; child = child->next) {
                    std::string key(child->key, child->keySize);
                    V item = V();
                    decodeValue(key.c_str(), item, NULL);
                    value.insert(std::pair<K, V>(internal::STOT::type<K>::strto(key.c_str()), item));
                }
                _cur = parent;
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }
        
        FORCEINLINE void decodeValue(const char* name, bool& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_BOOL) {
                if (item->valueSize == 4) {
                    value = true;
                } else if (item->valueSize == 5) {
                    value = false;
                } else {
                    assert(false);
                }
            }
        }

        FORCEINLINE void decodeValue(const char* name, uint32_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_NUMBER) {
                value = (uint32_t)custom::CustomGenericReader::convertUint(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, int32_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_NUMBER) {
                value = (int32_t)custom::CustomGenericReader::convertInt(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, uint64_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_NUMBER) {
                value = custom::CustomGenericReader::convertUint(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, int64_t& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_NUMBER) {
                value = custom::CustomGenericReader::convertInt(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, float& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_NUMBER) {
                value = (float)custom::CustomGenericReader::convertDouble(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, double& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_NUMBER) {
                value = custom::CustomGenericReader::convertDouble(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, std::string& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_STRING && item->value && item->valueSize) {
                value.clear();
                value.append(item->value, item->valueSize);
            }
        }

        FORCEINLINE void decodeValue(const char* name, std::vector<bool>& value, bool* pHas) {
            const custom::GenericValue* item = custom::GetObjectItem(_cur, name);
            if (item && item->type == custom::VALUE_ARRAY) {
                for (const custom::GenericValue* child = item->child; child && child->type == custom::VALUE_BOOL; child = child->next) {
                    value.push_back((child->valueSize == 4));
                }
            }
        }
        
    };

}


#endif
