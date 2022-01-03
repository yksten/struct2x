#ifndef __JSON_DECODER_H__
#define __JSON_DECODER_H__

#include <map>
#include <struct2x/traits.h>
#include <struct2x/struct2x.h>
#include <struct2x/json/GenericReader.h>


namespace struct2x {

    class EXPORTAPI JSONDecoder {
        bool _convertByType;                // convert by field type
        bool _caseInsensitive;              // key case insensitive
        custom::GenericReader _reader;
        const custom::GenericValue* _cur;
    public:
        JSONDecoder(const char* str, bool caseInsensitive = false);
        
        // convert by field type
        JSONDecoder& setConvertByType(bool convertByType);
        
        template<typename T>
        JSONDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        JSONDecoder& convert(const char* name, T& value, bool* pHas = NULL) {
            decodeValue(name, *(typename internal::TypeTraits<T>::Type*)(&value), pHas);
            return *this;
        }

        template<typename T>
        bool operator>>(T& value) {
            if (!_cur) return false;
            const custom::GenericValue* parent = _cur;
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            return (parent == _cur);
        }
        
        template<typename T>
        bool operator>>(std::vector<T>& value) {
            if (!_cur) return false;
            const custom::GenericValue* parent = _cur;
            if (_cur) {
                uint32_t size = custom::GenericReader::GetObjectSize(_cur);
                if (size) {
                    value.resize(size);
                }
                const custom::GenericValue* parentTemp = _cur;
                _cur = _cur->child;
                for (uint32_t idx = 0; _cur && (idx < size); (_cur = _cur->next) && ++idx) {
                    decodeValue(NULL, *(typename internal::TypeTraits<T>::Type*)(&value.at(idx)), NULL);
                }
                _cur = parentTemp;
            }
            return (parent == _cur);
        }
        
        template<typename K, typename V>
        bool operator>>(std::map<K, V>& value) {
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
    private:
        template<typename T>
        void decodeValue(const char* name, T& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
            if (_cur) {
                internal::serializeWrapper(*this, value);
                if (pHas) *pHas = true;
            }
            _cur = parent;
        }

        template<typename T>
        void decodeValue(const char* name, std::vector<T>& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
            if (_cur) {
                uint32_t size = custom::GenericReader::GetObjectSize(_cur);
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
        void decodeValue(const char* name, std::map<K, V>& value, bool* pHas) {
            const custom::GenericValue* parent = _cur;
            _cur = custom::GenericReader::GetObjectItem(_cur, name, _caseInsensitive);
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
        
        void decodeValue(const char* name, bool& value, bool* pHas);
        void decodeValue(const char* name, uint32_t& value, bool* pHas);
        void decodeValue(const char* name, int32_t& value, bool* pHas);
        void decodeValue(const char* name, uint64_t& value, bool* pHas);
        void decodeValue(const char* name, int64_t& value, bool* pHas);
        void decodeValue(const char* name, float& value, bool* pHas);
        void decodeValue(const char* name, double& value, bool* pHas);
        void decodeValue(const char* name, std::string& value, bool* pHas);
        void decodeValue(const char* name, std::vector<bool>& value, bool* pHas);
        bool checkItemType(const custom::GenericValue& item, const int type) const;
        bool item2Bool(const custom::GenericValue& item) const;
        
    };

}


#endif
