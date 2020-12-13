#ifndef __PROTOBUF_ENCODER_H__
#define __PROTOBUF_ENCODER_H__

#include <string>
#include <map>
#include "serialize.h"

#define BEGINCALCULATEFIELD(buf) \
    std::pair<bool, size_t> customField(buf.getCustomField());

#define ENDCALCULATEFIELD(nByteSize, buf) \
    nByteSize = buf.getCustomField().second; buf.setCustomField(customField);

namespace serialize {

    class EXPORTAPI BufferWrapper {
        std::string* _buffer;
        uint8_t* _target;
        mutable bool _bCalculateFlag;
        mutable uint32_t _cumtomFieldSize;

        BufferWrapper(const BufferWrapper&);
        BufferWrapper& operator=(const BufferWrapper&);
    public:
        explicit BufferWrapper(std::string* str);

        size_t size() const;
        std::string* buffer();
        uint8_t*& target();
        void reserve(uint32_t nSize);
        void appendBytes(const void* data, size_t len);
        void appendLength(uint32_t nLength);

        bool isGetLength() const;

        std::pair<bool, uint32_t> getCustomField() const;
        void setCustomField(const std::pair<bool, uint32_t>& pair);
    };

    class EXPORTAPI PBEncoder {

        struct enclosure_t {
            enclosure_t(uint32_t n, bool* b) :size(n), pHas(b) { memset(sz, 0, 10); }
            uint8_t sz[10];
            uint32_t size;
            bool* pHas;
        };

        class convertMgr {
            typedef void(*convert_t)(const void*, const enclosure_t&, BufferWrapper&);
            typedef size_t offset_type;
            class converter {
                convert_t _func;
                offset_type _offset;
                enclosure_t _info;
            public:
                converter(convert_t func, uint64_t tag, offset_type offset, bool* pHas)
                    :_func(func), _offset(offset), _info(encodeVarint(tag, pHas)) {
                }
                void operator()(const void* cValue, BufferWrapper& buf) const {
                    (*_func)((const uint8_t*)cValue + _offset, _info, buf);
                }
            };
            const uint8_t* _struct;
            std::vector<converter> _functionSet;
        public:
            bool empty() const { return _functionSet.empty(); }
            void setStruct(const void* pStruct) { _struct = (const uint8_t*)pStruct; }

            template<typename T>
            void bindValue(void(*f)(const T&, const enclosure_t&, BufferWrapper&), const serializeItem<T>& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::isMessage<T>::WRITE_TYPE;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename T>
            void bindCustom(void(*f)(const T&, const enclosure_t&, BufferWrapper&), const serializeItem<T>& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::isMessage<T>::WRITE_TYPE;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename T>
            void bindArray(void(*f)(const std::vector<T>&, const enclosure_t&, BufferWrapper&), const serializeItem<std::vector<T> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::isMessage<T>::WRITE_TYPE;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename T>
            void bindPack(void(*f)(const std::vector<T>&, const enclosure_t&, BufferWrapper&), const serializeItem<std::vector<T> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename K, typename V>
            void bindMap(void(*f)(const std::map<K, V>&, const enclosure_t&, BufferWrapper&), const serializeItem<std::map<K, V> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            uint32_t getByteSize(const void* pStruct, BufferWrapper& buf) {
                uint32_t nByteSize = 0;
                do {
                    BEGINCALCULATEFIELD(buf);
                    uint32_t nSize = _functionSet.size();
                    for (uint32_t idx = 0; idx < nSize; ++idx) {
                        (_functionSet[idx])(pStruct, buf);
                    }
                    ENDCALCULATEFIELD(nByteSize, buf);
                } while (0);
                return nByteSize;
            }

            void doConvert(const void* pStruct, BufferWrapper& buf) {
                uint32_t nSize = _functionSet.size();
                for (uint32_t idx = 0; idx < nSize; ++idx) {
                    (_functionSet[idx])(pStruct, buf);
                }
            }
        };

        BufferWrapper _buffer;
        convertMgr* _mgr;
        typedef void(*encodeFunction32)(const std::vector<uint32_t>&, const enclosure_t&, BufferWrapper&);
        static encodeFunction32 convsetSet32[3];
        static encodeFunction32 convsetSetPack32[3];
        typedef void(*encodeFunction64)(const std::vector<uint64_t>&, const enclosure_t&, BufferWrapper&);
        static encodeFunction64 convsetSet64[4];
        static encodeFunction64 convsetSetPack64[4];
    public:
        explicit PBEncoder(std::string& str);
        ~PBEncoder();

        template<typename T>
        static PBEncoder::convertMgr& getMessage(T& value) {
            static convertMgr mgr;
            if (mgr.empty()) {
                static PBEncoder encoder(*(std::string*)(NULL));
                encoder._mgr = &mgr;
                mgr.setStruct(&value);
                internal::serializeWrapper(encoder, value);
            }
            return mgr;
        }

        template<typename T>
        bool operator<<(const T& value) {
            convertMgr& mgr = getMessage(*const_cast<T*>(&value));
            uint32_t nByteSize = mgr.getByteSize((const uint8_t*)&value, _buffer);
            _buffer.reserve(nByteSize);
            mgr.doConvert((const uint8_t*)&value, _buffer);
            if (_buffer.size() != nByteSize) {
                //ByteSizeConsistencyError
                return false;
            }
            return true;
        }

        template<typename T>
        PBEncoder& operator&(const serializeItem<T>& value) {
            encodeField(*(const serializeItem<typename internal::TypeTraits<T>::Type>*)(&value));
            return *this;
        }

        template<typename T>
        PBEncoder& operator&(const serializeItem<std::vector<T> >& value) {
            if (value.type >> 16) {
                _mgr->bindPack(&PBEncoder::encodeValuePack, *(const serializeItem<std::vector<typename internal::TypeTraits<T>::Type> >*)(&value));
            } else {
                _mgr->bindArray(&PBEncoder::encodeValue, *(const serializeItem<std::vector<typename internal::TypeTraits<T>::Type> >*)(&value));
            }
            return *this;
        }

        PBEncoder& operator&(const serializeItem<std::vector<int32_t> >& value);
        PBEncoder& operator&(const serializeItem<std::vector<uint32_t> >& value);
        PBEncoder& operator&(const serializeItem<std::vector<int64_t> >& value);
        PBEncoder& operator&(const serializeItem<std::vector<uint64_t> >& value);

        template<typename K, typename V>
        PBEncoder& operator&(const serializeItem<std::map<K, V> >& value) {
            _mgr->bindMap(&PBEncoder::encodeValue, value);
            return *this;
        }
        template<typename V> PBEncoder& operator&(const serializeItem<std::map<float, V> >& value);
        template<typename V> PBEncoder& operator&(const serializeItem<std::map<double, V> >& value);

    private:
        template<typename T>
        void encodeField(const serializeItem<T>& value) {
            _mgr->bindCustom(&PBEncoder::encodeValue, value);
        }
        void encodeField(const serializeItem<bool>& value);
        void encodeField(const serializeItem<int32_t>& value);
        void encodeField(const serializeItem<uint32_t>& value);
        void encodeField(const serializeItem<int64_t>& value);
        void encodeField(const serializeItem<uint64_t>& value);
        void encodeField(const serializeItem<float>& value);
        void encodeField(const serializeItem<double>& value);
        void encodeField(const serializeItem<std::string>& value);

        static void varInt(uint64_t value, BufferWrapper& buf);
        static void svarInt(uint64_t value, BufferWrapper& buf);
        static void fixed32(uint32_t value, BufferWrapper& buf);
        static void fixed64(uint64_t value, BufferWrapper& buf);
        static enclosure_t encodeVarint(uint64_t tag, bool* pHas);

        static void encodeValue(const bool& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const int32_t& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const uint32_t& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const int64_t& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const uint64_t& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const float& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const double& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const std::string& v, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueSvarint(const uint32_t& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed32(const uint32_t& v, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueSvarint(const uint64_t& v, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed64(const uint64_t& v, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueVarint(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarint(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed32(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueVarint(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarint(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed64(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueVarintPack(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarintPack(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed32Pack(const std::vector<uint32_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueVarintPack(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarintPack(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed64Pack(const std::vector<uint64_t>& value, const enclosure_t& info, BufferWrapper& buf);

        template<typename T>
        static void encodeValue(const T& v, const enclosure_t& info, BufferWrapper& buf) {
            buf.appendBytes(info.sz, info.size);
            convertMgr& mgr = getMessage(*const_cast<T*>(&v));
            uint32_t nByteSize = mgr.getByteSize((const uint8_t*)&v, buf);
            varInt(nByteSize, buf);
            if (buf.isGetLength()) {
                buf.appendLength(nByteSize);
            } else {
                mgr.doConvert((const uint8_t*)&v, buf);
            }
        }

        template<typename T>
        static void encodeValue(const std::vector<T>& value, const enclosure_t& info, BufferWrapper& buf) {
            if (!value.empty()) {
                uint32_t size = (uint32_t)value.size();
                for (uint32_t i = 0; i < size; ++i) {
                    encodeValue(*(const typename internal::TypeTraits<T>::Type*)(&value.at(i)), info, buf);
                }
            }
        }

        template<typename T>
        static void encodeValuePack(const std::vector<T>& value, const enclosure_t& info, BufferWrapper& buf) {
            if (!value.empty()) {
                buf.appendBytes(info.sz, info.size);
                uint32_t size = (uint32_t)value.size();
                for (uint32_t i = 0; i < size; ++i) {
                    encodeValue(*(const typename internal::TypeTraits<T>::Type*)(&value.at(i)), info, buf);
                }
            }
        }

        template<typename K, typename V>
        static void encodeValue(const std::map<K, V>& value, const enclosure_t& info, BufferWrapper& buf) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                buf.appendBytes(info.sz, info.size);
                static enclosure_t infok = encodeVarint(((uint64_t)1 << 3) | internal::isMessage<K>::WRITE_TYPE, NULL);
                static enclosure_t infov = encodeVarint(((uint64_t)2 << 3) | internal::isMessage<V>::WRITE_TYPE, NULL);
                uint32_t nByteSize = 0;
                do {
                    BEGINCALCULATEFIELD(buf);
                    encodeValue(*(const typename internal::TypeTraits<K>::Type*)(&it->first), infok, buf);
                    encodeValue(*(const typename internal::TypeTraits<V>::Type*)(&it->second), infov, buf);
                    ENDCALCULATEFIELD(nByteSize, buf);
                } while (0);
                varInt(nByteSize, buf);
                if (buf.isGetLength()) {
                    buf.appendLength(nByteSize);
                } else {
                    encodeValue(*(const typename internal::TypeTraits<K>::Type*)(&it->first), infok, buf);
                    encodeValue(*(const typename internal::TypeTraits<V>::Type*)(&it->second), infov, buf);
                }
            }
        }

    };

}


#endif