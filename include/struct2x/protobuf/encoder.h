#ifndef __PROTOBUF_ENCODER_H__
#define __PROTOBUF_ENCODER_H__

#include <assert.h>
#include <string>
#include <map>
#include <struct2x/struct2x.h>
#include <struct2x/traits.h>

#define BEGINCALCULATEFIELD(buf) \
    std::pair<bool, size_t> customField(buf.getCustomField());

#define ENDCALCULATEFIELD(nByteSize, buf) \
    nByteSize = buf.getCustomField().second; buf.setCustomField(customField);

namespace struct2x {

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
            enclosure_t(uint32_t t, uint32_t n) : type(t), size(n) { memset(sz, 0, 10); }
            uint32_t type;
            uint8_t sz[10];
            uint32_t size;
        };

        class convertMgr {
            typedef void(*convert_t)(const void*, const bool*, const enclosure_t&, BufferWrapper&);
            typedef size_t offset_type;
            class converter {
                convert_t _func;
                offset_type _offset;
                offset_type _has;
                enclosure_t _info;
            public:
                converter(convert_t func, uint64_t tag, uint32_t type, offset_type offset, offset_type has)
                    :_func(func), _offset(offset), _has(has), _info(encodeVarint(tag, type)) {
                }
                void operator()(const void* pStruct, BufferWrapper& buf) const {
                    (*_func)((const uint8_t*)pStruct + _offset, (const bool*)(_has ? (const uint8_t*)pStruct + _has : NULL), _info, buf);
                }
            };
            const uint8_t* _struct;
            std::vector<converter> _functionSet;
        public:
            explicit convertMgr(const void* pStruct = NULL) :_struct((const uint8_t*)pStruct) {}
            convertMgr(const convertMgr& that) :_struct(NULL), _functionSet(that._functionSet) {}
            bool empty() const { return _functionSet.empty(); }

            void bindValue(void(*f)(const std::string&, const bool*, const enclosure_t&, BufferWrapper&), const serializeItem<std::string>& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                offset_type has = value.bHas ? ((const uint8_t*)(value.bHas)) - _struct : 0;
                _functionSet.push_back(converter(convert_t(f), tag, value.type, offset, has));
            }
            template<typename T>
            void bindValue(void(*f)(const T&, const bool*, const enclosure_t&, BufferWrapper&), const serializeItem<T>& value) {
                uint64_t type = internal::isMessage<T>::WRITE_TYPE;
                if ((value.type & 0xFFFF) == TYPE_FIXED32) {
                    type = internal::WT_32BIT;
                } else if ((value.type & 0xFFFF) == TYPE_FIXED64) {
                    type = internal::WT_64BIT;
                }
                uint64_t tag = ((uint64_t)value.num << 3) | type;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                offset_type has = value.bHas ? ((const uint8_t*)(value.bHas)) - _struct : 0;
                _functionSet.push_back(converter(convert_t(f), tag, value.type, offset, has));
            }

            template<typename T>
            void bindCustom(void(*f)(const T&, const bool*, const enclosure_t&, BufferWrapper&), const serializeItem<T>& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                offset_type has = value.bHas ? ((const uint8_t*)(value.bHas)) - _struct : 0;
                _functionSet.push_back(converter(convert_t(f), tag, value.type, offset, has));
            }

            template<typename T>
            void bindArray(void(*f)(const std::vector<T>&, const bool*, const enclosure_t&, BufferWrapper&), const serializeItem<std::vector<T> >& value) {
                uint64_t type = internal::isMessage<T>::WRITE_TYPE;
                if ((value.type & 0xFFFF) == TYPE_FIXED32) {
                    type = internal::WT_32BIT;
                } else if ((value.type & 0xFFFF) == TYPE_FIXED64) {
                    type = internal::WT_64BIT;
                }
                uint64_t tag = ((uint64_t)value.num << 3) | type;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                offset_type has = value.bHas ? ((const uint8_t*)(value.bHas)) - _struct : 0;
                _functionSet.push_back(converter(convert_t(f), tag, value.type, offset, has));
            }

            template<typename T>
            void bindPack(void(*f)(const std::vector<T>&, const bool*, const enclosure_t&, BufferWrapper&), const serializeItem<std::vector<T> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                offset_type has = value.bHas ? ((const uint8_t*)(value.bHas)) - _struct : 0;
                _functionSet.push_back(converter(convert_t(f), tag, value.type, offset, has));
            }

            template<typename K, typename V>
            void bindMap(void(*f)(const std::map<K, V>&, const bool*, const enclosure_t&, BufferWrapper&), const serializeItem<std::map<K, V> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                offset_type has = value.bHas ? ((const uint8_t*)(value.bHas)) - _struct : 0;
                _functionSet.push_back(converter(convert_t(f), tag, value.type, offset, has));
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
        typedef void(*encodeFunction32)(const std::vector<uint32_t>&, const bool*, const enclosure_t&, BufferWrapper&);
        static encodeFunction32 convsetSet32[3];
        static encodeFunction32 convsetSetPack32[3];
        typedef void(*encodeFunction64)(const std::vector<uint64_t>&, const bool*, const enclosure_t&, BufferWrapper&);
        static encodeFunction64 convsetSet64[4];
        static encodeFunction64 convsetSetPack64[4];
    public:
        explicit PBEncoder(std::string& str);
        ~PBEncoder();

        template<typename T>
        static PBEncoder::convertMgr getMessage(T& value) {
            convertMgr mgr(&value);
            if (mgr.empty()) {
                std::string str;
                PBEncoder encoder(str);
                encoder._mgr = &mgr;
                internal::serializeWrapper(encoder, value);
            }
            return mgr;
        }

        template<typename T>
        bool operator<<(const T& value) {
            static convertMgr mgr = getMessage(*const_cast<T*>(&value));
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
            if (value.type & 0xFFFF) {
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
        static void svarInt(uint32_t value, BufferWrapper& buf);
        static void svarInt(uint64_t value, BufferWrapper& buf);
        static void fixed32(uint32_t value, BufferWrapper& buf);
        static void fixed64(uint64_t value, BufferWrapper& buf);
        static enclosure_t encodeVarint(uint64_t tag, uint32_t type);

        static void encodeValue(const bool& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const int32_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const uint32_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const int64_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const uint64_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const float& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const double& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValue(const std::string& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueSvarint32(const uint32_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarint64(const uint64_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueFixed32(const uint32_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed64(const uint64_t& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueVarintArray(const std::vector<uint32_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarintArray(const std::vector<uint32_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed32Array(const std::vector<uint32_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueVarintArray(const std::vector<uint64_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarintArray(const std::vector<uint64_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed64Array(const std::vector<uint64_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);

        static void encodeValueVarintPack(const std::vector<uint32_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarintPack(const std::vector<uint32_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed32Pack(const std::vector<uint32_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueVarintPack(const std::vector<uint64_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueSvarintPack(const std::vector<uint64_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);
        static void encodeValueFixed64Pack(const std::vector<uint64_t>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf);

        template<typename T>
        static void encodeValue(const T& v, const bool* pHas, const enclosure_t& info, BufferWrapper& buf) {
            buf.appendBytes(info.sz, info.size);
            static convertMgr mgr = getMessage(*const_cast<T*>(&v));
            uint32_t nByteSize = mgr.getByteSize((const uint8_t*)&v, buf);
            varInt(nByteSize, buf);
            if (buf.isGetLength()) {
                buf.appendLength(nByteSize);
            } else {
                mgr.doConvert((const uint8_t*)&v, buf);
            }
        }

        template<typename T>
        static void encodeValue(const std::vector<T>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf) {
            if (!value.empty()) {
                uint32_t size = (uint32_t)value.size();
                for (uint32_t i = 0; i < size; ++i) {
                    encodeValue(*(const typename internal::TypeTraits<T>::Type*)(&value.at(i)), NULL, info, buf);
                }
            }
        }

        template<typename T>
        static void encodeValuePack(const std::vector<T>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf) {
            if (!value.empty()) {
                buf.appendBytes(info.sz, info.size);
                uint32_t size = (uint32_t)value.size();
                for (uint32_t i = 0; i < size; ++i) {
                    encodeValue(*(const typename internal::TypeTraits<T>::Type*)(&value.at(i)), NULL, info, buf);
                }
            }
        }

        template<typename K, typename V>
        static void encodeValue(const std::map<K, V>& value, const bool* pHas, const enclosure_t& info, BufferWrapper& buf) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                buf.appendBytes(info.sz, info.size);
                static enclosure_t infok = encodeVarint(((uint64_t)1 << 3) | internal::isMessage<K>::WRITE_TYPE, TYPE_VARINT);
                static enclosure_t infov = encodeVarint(((uint64_t)2 << 3) | internal::isMessage<V>::WRITE_TYPE, TYPE_VARINT);
                uint32_t nByteSize = 0;
                do {
                    BEGINCALCULATEFIELD(buf);
                    encodeValue(*(const typename internal::TypeTraits<K>::Type*)(&it->first), NULL, infok, buf);
                    encodeValue(*(const typename internal::TypeTraits<V>::Type*)(&it->second), NULL, infov, buf);
                    ENDCALCULATEFIELD(nByteSize, buf);
                } while (0);
                varInt(nByteSize, buf);

                uint32_t keyType = info.type >> BITNUM;
                uint32_t valueType = info.type & 0xFFFF;
                typedef void(*encodeFunction)(const void*, const bool*, const enclosure_t&, BufferWrapper&);
                static encodeFunction convsetSet[] = { NULL, (encodeFunction)&PBEncoder::encodeValueSvarint32
                    , (encodeFunction)&PBEncoder::encodeValueFixed32, (encodeFunction)&PBEncoder::encodeValueFixed64 };
                if (keyType == TYPE_VARINT) {
                    encodeValue(*(const typename internal::TypeTraits<K>::Type*)(&it->first), NULL, infok, buf);
                } else if (keyType == TYPE_SVARINT || keyType == TYPE_FIXED32 || keyType == TYPE_FIXED64) {
                    convsetSet[keyType](&it->first, NULL, infok, buf);
                }
                if (valueType == TYPE_VARINT) {
                    encodeValue(*(const typename internal::TypeTraits<V>::Type*)(&it->second), NULL, infov, buf);
                } else if (valueType == TYPE_SVARINT || valueType == TYPE_FIXED32 || valueType == TYPE_FIXED64) {
                    convsetSet[valueType](&it->second, NULL, infov, buf);
                }
            }
        }

    };

}


#endif
