#ifndef __PROTOBUF_ENCODER_H__
#define __PROTOBUF_ENCODER_H__

#include <string>
#include <map>
#include "serialize.h"


namespace serialize {

    class EXPORTAPI BufferWrapper {
        std::string* _buffer;
        bool _bCalculateFlag;
        size_t _cumtomFieldSize;
    public:
        explicit BufferWrapper(std::string* str);

        const uint8_t* data() const;
        size_t size() const;
        std::string* buffer();
        void append(const void* data, size_t len);

        bool isGetLength() const;
        void append(uint32_t nLength);
        void startCalculateSize();
        std::pair<bool, size_t> getCustomField() const;
        void setCustomField(const std::pair<bool, size_t>& pair);
    };

    class EXPORTAPI PBEncoder {

        class calculateFieldHelper {
            BufferWrapper& _buff;
            size_t& _nSize;
            std::pair<bool, size_t> _customField;
        public:
            calculateFieldHelper(BufferWrapper& buff, size_t& nSize)
                :_buff(buff), _nSize(nSize), _customField(_buff.getCustomField()) {
                _buff.startCalculateSize();
            }
            ~calculateFieldHelper() {
                _nSize = _buff.getCustomField().second;
                _buff.setCustomField(_customField);
            }
        };

        struct enclosure_type {
            enclosure_type(uint32_t n, bool* b) :size(n), pHas(b) { memset(sz, 0, 10); }
            uint8_t sz[10];
            uint32_t size;
            bool* pHas;
        };

        class convertMgr {
            typedef void(*convert_t)(const void*, const enclosure_type&, BufferWrapper&);
            typedef size_t offset_type;
            class converter {
                convert_t _func;
                offset_type _offset;
                enclosure_type _info;
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
            void setStruct(const void* pStruct) { _struct = (const uint8_t*)pStruct; }

            template<typename T>
            void bindValue(void(*f)(const T&, const enclosure_type&, BufferWrapper&), const serializeItem<T>& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::isMessage<T>::WRITE_TYPE;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename T>
            void bindCustom(void(*f)(const T&, const enclosure_type&, BufferWrapper&), const serializeItem<T>& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::isMessage<T>::WRITE_TYPE;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename T>
            void bindArray(void(*f)(const std::vector<T>&, const enclosure_type&, BufferWrapper&), const serializeItem<std::vector<T> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::isMessage<T>::WRITE_TYPE;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename T>
            void bindPack(void(*f)(const std::vector<T>&, const enclosure_type&, BufferWrapper&), const serializeItem<std::vector<T> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            template<typename K, typename V>
            void bindMap(void(*f)(const std::map<K, V>&, const enclosure_type&, BufferWrapper&), const serializeItem<std::map<K, V> >& value) {
                uint64_t tag = ((uint64_t)value.num << 3) | internal::WT_LENGTH_DELIMITED;
                offset_type offset = ((const uint8_t*)(&value.value)) - _struct;
                _functionSet.push_back(converter(convert_t(f), tag, offset, value.bHas));
            }

            void doConvert(const void* pStruct, BufferWrapper& buf) {
                for (uint32_t idx = 0; idx < _functionSet.size(); ++idx) {
                    (_functionSet[idx])(pStruct, buf);
                }
            }
        };

        BufferWrapper _buffer;
        convertMgr* _mgr;
        typedef void(*encodeFunction32)(const std::vector<uint32_t>&, const enclosure_type&, BufferWrapper&);
        static encodeFunction32 convsetSet32[3];
        static encodeFunction32 convsetSetPack32[3];
        typedef void(*encodeFunction64)(const std::vector<uint64_t>&, const enclosure_type&, BufferWrapper&);
        static encodeFunction64 convsetSet64[4];
        static encodeFunction64 convsetSetPack64[4];
    public:
        explicit PBEncoder(std::string& str);
        ~PBEncoder();

        template<typename T>
        PBEncoder::convertMgr getMessage(T& value) {
            convertMgr mgr;
            _mgr = &mgr;
            mgr.setStruct(&value);
            internal::serializeWrapper(*this, value);
            return mgr;
        }

        template<typename T>
        bool operator<<(const T& value) {
            static convertMgr mgr = getMessage(*const_cast<T*>(&value));
            mgr.doConvert((const uint8_t*)&value, _buffer);
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
        static enclosure_type encodeVarint(uint64_t tag, bool* pHas);

        static void encodeValue(const bool& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const int32_t& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const uint32_t& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const int64_t& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const uint64_t& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const float& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const double& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValue(const std::string& v, const enclosure_type& info, BufferWrapper& buf);

        static void encodeValueSvarint(const uint32_t& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueFixed32(const uint32_t& v, const enclosure_type& info, BufferWrapper& buf);

        static void encodeValueSvarint(const uint64_t& v, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueFixed64(const uint64_t& v, const enclosure_type& info, BufferWrapper& buf);

        static void encodeValueVarint(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueSvarint(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueFixed32(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueVarint(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueSvarint(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueFixed64(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf);

        static void encodeValueVarintPack(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueSvarintPack(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueFixed32Pack(const std::vector<uint32_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueVarintPack(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueSvarintPack(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf);
        static void encodeValueFixed64Pack(const std::vector<uint64_t>& value, const enclosure_type& info, BufferWrapper& buf);

        template<typename T>
        static void encodeValue(const T& v, const enclosure_type& info, BufferWrapper& buf) {
            buf.append(info.sz, info.size);
            size_t nCustomFieldSize = 0;
            PBEncoder encoder(*buf.buffer());
            do {
                calculateFieldHelper h(encoder._buffer, nCustomFieldSize);
                encoder << v;
            } while (0);
            varInt(nCustomFieldSize, buf);
            if (buf.isGetLength()) {
                buf.append(nCustomFieldSize);
            } else {
                encoder << v;
            }
        }

        template<typename T>
        static void encodeValue(const std::vector<T>& value, const enclosure_type& info, BufferWrapper& buf) {
            if (!value.empty()) {
                uint32_t size = (uint32_t)value.size();
                for (uint32_t i = 0; i < size; ++i) {
                    encodeValue(*(const typename internal::TypeTraits<T>::Type*)(&value.at(i)), info, buf);
                }
            }
        }

        template<typename T>
        static void encodeValuePack(const std::vector<T>& value, const enclosure_type& info, BufferWrapper& buf) {
            if (!value.empty()) {
                buf.append(info.sz, info.size);
                uint32_t size = (uint32_t)value.size();
                for (uint32_t i = 0; i < size; ++i) {
                    encodeValue(*(const typename internal::TypeTraits<T>::Type*)(&value.at(i)), info, buf);
                }
            }
        }

        template<typename K, typename V>
        static void encodeValue(const std::map<K, V>& value, const enclosure_type& info, BufferWrapper& buf) {
            for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
                buf.append(info.sz, info.size);
                static enclosure_type infok = encodeVarint(((uint64_t)1 << 3) | internal::isMessage<K>::WRITE_TYPE, NULL);
                static enclosure_type infov = encodeVarint(((uint64_t)2 << 3) | internal::isMessage<V>::WRITE_TYPE, NULL);
                size_t nCustomFieldSize = 0;
                do {
                    calculateFieldHelper h(buf, nCustomFieldSize);
                    encodeValue(*(const typename internal::TypeTraits<K>::Type*)(&it->first), infok, buf);
                    encodeValue(*(const typename internal::TypeTraits<V>::Type*)(&it->second), infov, buf);
                } while (0);
                varInt(nCustomFieldSize, buf);

                encodeValue(*(const typename internal::TypeTraits<K>::Type*)(&it->first), infok, buf);
                encodeValue(*(const typename internal::TypeTraits<V>::Type*)(&it->second), infov, buf);
            }
        }

    };

}


#endif