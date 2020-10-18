#ifndef __MSGPACK_ENCODER_H__
#define __MSGPACK_ENCODER_H__

#include <string>
#include <map>
#include "struct2x.h"


namespace struct2x {

    class EXPORTAPI MPHelper {
        std::string& _buffer;
        bool _flag;
        uint32_t _fieldNum;
    public:
        MPHelper(std::string& buf);
        ~MPHelper();

        bool setKey(const char* sz);
        void setValue(bool v);
        void setValue(int32_t v);
        void setValue(uint32_t v);
        void setValue(int64_t v);
        void setValue(uint64_t v);
        void setValue(float v);
        void setValue(double v);
        void setValue(const std::string& v);
        void setMapSize(size_t n);
        void setArraySize(size_t n);

        bool flag() const { return _flag; }
        void flag(bool f) { _flag = f; }
        uint32_t fieldNum() const { return _fieldNum; }
        void fieldNum(uint32_t n) { _fieldNum = n; }

        template<typename T>
        bool checkEmpty(const T& v) {
            if (v.empty()) {
                static const unsigned char d = 0xc0;
                _buffer.append((const char*)&d, 1);
                return true;
            }
            return false;
        }
    };

    class EXPORTAPI MPEncoder {
        std::string& _buffer;
        MPHelper _helper;
        MPEncoder(const MPEncoder&);
        MPEncoder& operator=(const MPEncoder&);
    public:
        MPEncoder(std::string& buf);
        ~MPEncoder();

        template<typename T>
        MPEncoder& operator&(serializeItem<T> value) {
            return setValue(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        MPEncoder& operator << (const T& value) {
            uint32_t nFieldNum = _helper.fieldNum();
            bool bFlag = _helper.flag();
            _helper.flag(false);
            _helper.fieldNum(0);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            _helper.setMapSize(_helper.fieldNum());
            _helper.fieldNum(nFieldNum);
            _helper.flag(bFlag);
            internal::serializeWrapper(*this, *const_cast<T*>(&value));
            return *this;
        }

        template<typename T>
        MPEncoder& convert(const char* sz, const T& value, bool* pHas = NULL) {
            setValue(sz, *(typename internal::TypeTraits<T>::Type*)(&value));
            return *this;
        }

    private:
        template<typename T>
        void setValue(const char* sz, const T& value) {
            if (_helper.setKey(sz)) {
                uint32_t nFieldNum = _helper.fieldNum();
                bool bFlag = _helper.flag();
                _helper.flag(false);
                internal::serializeWrapper(*this, *const_cast<T*>(&value));
                _helper.setMapSize(_helper.fieldNum());
                _helper.fieldNum(nFieldNum);
                _helper.flag(bFlag);
                internal::serializeWrapper(*this, *const_cast<T*>(&value));
            }
        }

        template<typename T>
        void setValue(const char* sz, const std::vector<T>& value) {
            if (_helper.setKey(sz) && _helper.checkEmpty(value)) {
                size_t size = (int32_t)value.size();
                _helper.setArraySize(size);
                for (int32_t i = 0; i < size; ++i) {
                    uint32_t nFieldNum = _helper.fieldNum();
                    bool bFlag = _helper.flag();
                    _helper.flag(false);
                    internal::serializeWrapper(*this, *const_cast<T*>(&value[i]));
                    _helper.setMapSize(_helper.fieldNum());
                    _helper.fieldNum(nFieldNum);
                    _helper.flag(bFlag);
                    internal::serializeWrapper(*this, *const_cast<T*>(&value[i]));
                }
            }
        }

        template<typename K, typename V>
        void setValue(const char* sz, const std::map<K, V>& value) {
            if (_helper.setKey(sz) && _helper.checkEmpty(value)) {
                size_t size = (int32_t)value.size();
                _helper.setMapSize(size);
                for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                    setValue(internal::STOT::type<K>::tostr(it->first), it->sencond);
                }
            }
        }

        void setValue(const char* sz, bool value);
        void setValue(const char* sz, uint32_t value);
        void setValue(const char* sz, int32_t value);
        void setValue(const char* sz, uint64_t value);
        void setValue(const char* sz, int64_t value);
        void setValue(const char* sz, float value);
        void setValue(const char* sz, double value);
        void setValue(const char* sz, const std::string& value);
        void setValue(const char* sz, const std::vector<bool>& value);
        void setValue(const char* sz, const std::vector<uint32_t>& value);
        void setValue(const char* sz, const std::vector<int32_t>& value);
        void setValue(const char* sz, const std::vector<uint64_t>& value);
        void setValue(const char* sz, const std::vector<int64_t>& value);
        void setValue(const char* sz, const std::vector<float>& value);
        void setValue(const char* sz, const std::vector<double>& value);
        void setValue(const char* sz, const std::vector<std::string>& value);
    };

}


#endif