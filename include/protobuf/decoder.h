#ifndef __PROTOBUF_DECODER_H__
#define __PROTOBUF_DECODER_H__
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include "struct2x.h"

namespace proto {
    class Message;
}

namespace struct2x {

    class EXPORTAPI PBDecoder {
        proto::Message* _rootMsg;
        proto::Message* _curMsg;
        bool _bParseRet;

        template <int isStruct>
        struct valueDecoder {
            template <typename OUT>
            static void decode(serializeItem<OUT>& out, PBDecoder& decoder) {
                proto::Message* msg = decoder.getCurMsg(); {
                    decoder.setCurMsg(decoder.getMessage(out.num()));
                    decoder.operator>>(out.value());
                } decoder.setCurMsg(msg);
            }

            template <typename T>
            static void decodeRepaeted(serializeItem<std::vector<T> >& out, PBDecoder& decoder) {
                proto::Message* msg = decoder.getCurMsg();
                std::vector<proto::Message*>* repaetedMsg = decoder.getMessageArray(out.num());
                if (!repaetedMsg || repaetedMsg->empty()) return;
                for (uint32_t idx = 0; idx < repaetedMsg->size(); ++idx) {
                    decoder.setCurMsg(repaetedMsg->at(idx));
                    typename TypeTraits<T>::Type item = typename TypeTraits<T>::Type();
                    decoder.operator>>(item);
                    out.value().push_back(item);
                }
                decoder.setCurMsg(msg);
            }
        };
        template <>
        struct valueDecoder<0> {
            template <typename OUT>
            static void decode(serializeItem<OUT>& out, PBDecoder& decoder) { decoder.decodeValue(out); }

            template <typename OUT>
            static void decodeRepaeted(serializeItem<OUT>& out, PBDecoder& decoder) {
                decoder.decodeRepaeted(out);
            }
        };
        friend struct valueDecoder<0>;
        friend struct valueDecoder<1>;

        PBDecoder(const PBDecoder&);
        PBDecoder& operator=(const PBDecoder&);
    public:
        PBDecoder(const uint8_t* sz, uint32_t size);
        ~PBDecoder();

        template<typename T>
        bool operator>>(T& value) {
            if (_bParseRet)
                serializeWrapper(*this, value);
            return _bParseRet;
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<T> value) {
            valueDecoder<isMessage<T>::YES>::decode(value, *this);
            return *this;
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<std::vector<T> > value) {
            if (!value.value().empty()) value.value().clear();
            valueDecoder<isMessage<T>::YES>::decodeRepaeted(value, *this);
            return *this;
        }

        template<typename K, typename V>
        PBDecoder& operator&(serializeItem<std::map<K, V> > value) {
            if (!value.value().empty()) value.value().clear();
            proto::Message* msg = getCurMsg();
            std::vector<proto::Message*>* repaetedMsg = getMessageArray(value.num());
            if (repaetedMsg) {
                for (uint32_t idx = 0; idx < repaetedMsg->size(); ++idx) {
                    setCurMsg(repaetedMsg->at(idx));
                    typename K key = typename K();
                    decodeValue(SERIALIZE(1, key));
                    typename V v = typename V();
                    valueDecoder<isMessage<V>::YES>::decode(SERIALIZE(2, v), *this);
                    value.value().insert(std::pair<K, V>(key, v));
                }
            }
            setCurMsg(msg);
            return *this;
        }
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<float, V> > value);
        template<typename V>
        PBDecoder& operator&(serializeItem<std::map<double, V> > value);
    private:
        proto::Message* getMessage(int32_t number);
        std::vector<proto::Message*>* getMessageArray(int32_t number);

        template<typename T>
        void decodeValue(serializeItem<T>& v);

        template<typename T>
        void decodeRepaeted(serializeItem<std::vector<T> >& v);

        proto::Message* getCurMsg() { return _curMsg; }
        void setCurMsg(proto::Message* msg) { _curMsg = msg; }
    };

    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<bool>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<int32_t>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<uint32_t>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<int64_t>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<uint64_t>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<float>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<double>&);
    template<> EXPORTAPI void PBDecoder::decodeValue(serializeItem<std::string>&);

    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<bool> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<int32_t> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint32_t> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<int64_t> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint64_t> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<float> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<double> >&);
    template<> EXPORTAPI void PBDecoder::decodeRepaeted(serializeItem<std::vector<std::string> >&);
}

#endif