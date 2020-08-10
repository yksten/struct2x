#ifndef __PROTOBUF_ENCODER_H__
#define __PROTOBUF_ENCODER_H__
#include <string>
#include <vector>
#include <map>
#include "struct2x.h"


namespace struct2x {

    class EXPORTAPI PBEncoder {
        typedef void(PBEncoder::*writeValue)(uint64_t);
        static writeValue const functionArray[4];
        template <int isStruct>
        struct valueEncoder {
            template <typename IN>
            static void encode(const IN& in, int32_t type, PBEncoder& encoder) {
                encoder.encodeValue(in, type);
            }
        };
        template <>
        struct valueEncoder<1> {
            template <typename IN>
            static void encode(const IN& in, int32_t type, PBEncoder& encoder) {
                encoder.operator<<(in);
            }
        };
        friend struct valueEncoder<1>;
        BufferWrapper& _buffer;
    public:
        PBEncoder(BufferWrapper& _buffer);
        ~PBEncoder();

        const char* data()const;
        uint32_t size()const;

        template<typename T>
        bool operator<<(const T& value) {
            T* pValue = const_cast<T*>(&value);
            serializeWrapper(*this, *pValue);
            return true;
        }

        template<typename T>
        PBEncoder& operator&(const serializeItem<T>& value) {
            if (!isMessage<T>::YES) {
                uint64_t tag = ((uint64_t)value.num() << 3) | isMessage<T>::WRITE_TYPE;
                varInt(tag);
                valueEncoder<isMessage<T>::YES>::encode(value.value(), value.type(), *this);
                return *this;
            }
            uint64_t tag = ((uint64_t)value.num() << 3) | WT_LENGTH_DELIMITED;
            varInt(tag);
            BufferWrapper bfTemp;
            bfTemp.swap(_buffer);
            valueEncoder<isMessage<T>::YES>::encode(value.value(), value.type(), *this);
            _buffer.swap(bfTemp);
            varInt(bfTemp.size());
            _buffer.append(bfTemp.data(), bfTemp.size());
            return *this;
        }

        template<typename T>
        PBEncoder& operator&(const serializeItem<std::vector<T> >& value) {
            if (!isMessage<T>::YES && value.type() == TYPE_PACK) {
                return encodeRepaetedPack(value);
            }

            uint64_t tag = ((uint64_t)value.num() << 3) | WT_LENGTH_DELIMITED;
            uint32_t size = (uint32_t)value.value().size();
            for (uint32_t i = 0; i < size; ++i) {
                varInt(tag);
                if (isMessage<T>::YES) {
                    BufferWrapper bfTemp;
                    bfTemp.swap(_buffer);
                    valueEncoder<isMessage<T>::YES>::encode(value.value().at(i), value.type(), *this);
                    _buffer.swap(bfTemp);
                    varInt(bfTemp.size());
                    _buffer.append(bfTemp.data(), bfTemp.size());
                } else {
                    valueEncoder<isMessage<T>::YES>::encode(value.value().at(i), value.type(), *this);
                }
            }
            return *this;
        }

        template<typename K, typename V>
        PBEncoder& operator&(const serializeItem<std::map<K, V> >& value) {
            uint64_t tag = ((uint64_t)value.num() << 3) | WT_LENGTH_DELIMITED;
            for (std::map<K, V>::const_iterator it = value.value().begin(); it != value.value().end(); ++it) {
                varInt(tag);
                BufferWrapper bfTemp;
                bfTemp.swap(_buffer);
                do {
                    //operator&(serializeItem<K>(1, *const_cast<K*>(&it->first)));
                    varInt(((uint64_t)1 << 3) | isMessage<K>::WRITE_TYPE);
                    encodeValue(it->first, TYPE_VARINT);
                    operator&(SERIALIZE(2, *const_cast<V*>(&it->second)));
                } while (0);
                _buffer.swap(bfTemp);
                varInt(bfTemp.size());
                _buffer.append(bfTemp.data(), bfTemp.size());
            }
            return *this;
        }
        template<typename V>
        PBEncoder& operator&(const serializeItem<std::map<float, V> >& value);
        template<typename V>
        PBEncoder& operator&(const serializeItem<std::map<double, V> >& value);
    private:
        template<typename T>
        PBEncoder& encodeRepaetedPack(const serializeItem<std::vector<T> >& value) {
            uint64_t tag = ((uint64_t)value.num() << 3) | value.type();
            varInt(tag);
            BufferWrapper bfTemp;
            bfTemp.swap(_buffer);
            uint32_t size = (uint32_t)value.value().size();
            for (uint32_t i = 0; i < size; ++i) {
                valueEncoder<isMessage<T>::YES>::encode(value.value().at(i), value.type(), *this);
            }
            _buffer.swap(bfTemp);
            varInt(bfTemp.size());
            _buffer.append(bfTemp.data(), bfTemp.size());
            return *this;
        }
        PBEncoder& encodeValue(const std::string& v, int32_t type);
        PBEncoder& encodeValue(const float& v, int32_t type);
        PBEncoder& encodeValue(const double& v, int32_t type);
        template<typename T>
        PBEncoder& encodeValue(const T& v, int32_t type) {
            value(v, type);
            return *this;
        }
        void value(uint64_t value, int32_t type);
        void varInt(uint64_t value);
        void svarInt(uint64_t value);
        void fixed32(uint64_t value);
        void fixed64(uint64_t value);
        void encodeVarint32(uint32_t low, uint32_t high);
    };
}


#endif