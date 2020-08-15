#ifndef __PROTOBUF_DECODER_H__
#define __PROTOBUF_DECODER_H__
#include <string>
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
        PBDecoder(const PBDecoder&);
        PBDecoder& operator=(const PBDecoder&);
    public:
        PBDecoder(const uint8_t* sz, uint32_t size);
        ~PBDecoder();

        template<typename T>
        bool operator>>(T& value) {
            if (_bParseRet)
                internal::serializeWrapper(*this, value);
            return _bParseRet;
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<T> value) {
            decodeValue(value);
            return *this;
        }

        template<typename T>
        PBDecoder& operator&(serializeItem<std::vector<T> > value) {
            if (!value.value.empty()) value.value.clear();
            decodeRepaeted(value);
            return *this;
        }

        template<typename K, typename V>
        PBDecoder& operator&(serializeItem<std::map<K, V> > value) {
            if (!value.value.empty()) value.value.clear();
            proto::Message* msg = getCurMsg();
            std::vector<proto::Message*>* repaetedMsg = getMessageArray(value.num);
            if (repaetedMsg) {
                for (uint32_t idx = 0; idx < repaetedMsg->size(); ++idx) {
                    setCurMsg(repaetedMsg->at(idx));
                    typename K key = typename K();
                    decodeValue(SERIALIZE(1, key));
                    typename V v = typename V();
                    decodeValue(SERIALIZE(2, v));
                    value.value.insert(std::pair<K, V>(key, v));
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
        void decodeValue(serializeItem<T>& v) {
            proto::Message* msg = getCurMsg(); 
            if (proto::Message* structMsg = getMessage(v.num)) {
                setCurMsg(structMsg);
                operator>>(v.value);
                v.setHas(true);
            }
            setCurMsg(msg);
        }
        void decodeValue(serializeItem<bool>&);
        void decodeValue(serializeItem<int32_t>&);
        void decodeValue(serializeItem<uint32_t>&);
        void decodeValue(serializeItem<int64_t>&);
        void decodeValue(serializeItem<uint64_t>&);
        void decodeValue(serializeItem<float>&);
        void decodeValue(serializeItem<double>&);
        void decodeValue(serializeItem<std::string>&);

        template<typename T>
        void decodeRepaeted(serializeItem<std::vector<T> >& v) {
            proto::Message* msg = getCurMsg();
            std::vector<proto::Message*>* repaetedMsg = getMessageArray(v.num);
            if (!repaetedMsg || repaetedMsg->empty()) return;
            for (uint32_t idx = 0; idx < repaetedMsg->size(); ++idx) {
                setCurMsg(repaetedMsg->at(idx));
                typename internal::TypeTraits<T>::Type item = typename internal::TypeTraits<T>::Type();
                operator>>(item);
                v.value.push_back(item);
            }
            setCurMsg(msg);
        }
        void decodeRepaeted(serializeItem<std::vector<bool> >&);
        void decodeRepaeted(serializeItem<std::vector<int32_t> >&);
        void decodeRepaeted(serializeItem<std::vector<uint32_t> >&);
        void decodeRepaeted(serializeItem<std::vector<int64_t> >&);
        void decodeRepaeted(serializeItem<std::vector<uint64_t> >&);
        void decodeRepaeted(serializeItem<std::vector<float> >&);
        void decodeRepaeted(serializeItem<std::vector<double> >&);
        void decodeRepaeted(serializeItem<std::vector<std::string> >&);

        proto::Message* getCurMsg() { return _curMsg; }
        void setCurMsg(proto::Message* msg) { _curMsg = msg; }
    };

}

#endif