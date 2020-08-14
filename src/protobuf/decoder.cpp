#include "protobuf/decoder.h"
#include <assert.h>
#include "parse/proto.h"

namespace struct2x {

    PBDecoder::PBDecoder(const uint8_t* sz, unsigned int size)
        : _rootMsg(new proto::Message)
        , _curMsg(_rootMsg)
        , _bParseRet(_rootMsg->ParseFromBytes(sz, size)) {
    }

    PBDecoder::~PBDecoder() {
        if (_rootMsg)
            delete _rootMsg;
    }

    proto::Message* PBDecoder::getMessage(int32_t number) {
        return _curMsg->GetMessage(number);
    }

    std::vector<proto::Message*>* PBDecoder::getMessageArray(int32_t number) {
        return _curMsg->GetMessageArray(number);
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<bool>& v) {
        if (!_curMsg) return;

        v.value() = (bool)_curMsg->GetVarInt(v.num());
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<int32_t>& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_SVARINT) {
            v.value() = (int32_t)_curMsg->GetSInt(v.num());
        } else if (v.type() == TYPE_FIXED32) {
            v.value() = (int32_t)_curMsg->GetFixedInt32(v.num());
        } else {
            v.value() = (int32_t)_curMsg->GetVarInt(v.num());
        }
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<uint32_t>& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_FIXED32) {
            v.value() = (uint32_t)_curMsg->GetFixedInt32(v.num());
        } else {
            v.value() = (uint32_t)_curMsg->GetVarInt(v.num());
        }
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<int64_t>& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_SVARINT) {
            v.value() = _curMsg->GetSInt(v.num());
        } else {
            v.value() = (int64_t)_curMsg->GetVarInt(v.num());
        }
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<uint64_t>& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_SVARINT) {
            v.value() = _curMsg->GetSInt(v.num());
        } else {
            v.value() = _curMsg->GetVarInt(v.num());
        }
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<float>& v) {
        if (!_curMsg) return;

        v.value() = _curMsg->GetFloat(v.num());
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<double>& v) {
        if (!_curMsg) return;

        v.value() = _curMsg->GetDouble(v.num());
    }

    template<>
    void PBDecoder::decodeValue(serializeItem<std::string>& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_BYTES) {
            proto::binType temp = _curMsg->GetBytes(v.num());
            v.value().clear();
            v.value().append((const char*)temp.first, temp.second);
        } else {
            v.value() = _curMsg->GetString(v.num());
        }
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<bool> >& v) {
        if (!_curMsg) return;

        _curMsg->GetVarIntArray(v.num(), v.value());
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<int32_t> >& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_SVARINT) {
            v.value() = _curMsg->GetSIntArray<int32_t>(v.num());
        } else if (v.type() == TYPE_FIXED32) {
            _curMsg->GetFixedInt32Array(v.num(), v.value());
        } else {
            _curMsg->GetVarIntArray(v.num(), v.value());
        }
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint32_t> >& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_FIXED32) {
            _curMsg->GetFixedInt32Array(v.num(), v.value());
        } else {
            _curMsg->GetVarIntArray(v.num(), v.value());
        }
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<int64_t> >& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_SVARINT) {
            v.value() = _curMsg->GetSIntArray<int64_t>(v.num());
        } else {
            _curMsg->GetVarIntArray(v.num(), v.value());
        }
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint64_t> >& v) {
        if (!_curMsg) return;

        _curMsg->GetVarIntArray(v.num(), v.value());
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<float> >& v) {
        if (!_curMsg) return;

        _curMsg->GetFloatArray(v.num(), v.value());
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<double> >& v) {
        if (!_curMsg) return;

        _curMsg->GetDoubleArray(v.num(), v.value());
    }

    template<>
    void PBDecoder::decodeRepaeted(serializeItem<std::vector<std::string> >& v) {
        if (!_curMsg) return;

        if (v.type() == TYPE_BYTES) {
            std::vector<proto::binType> temp;
            _curMsg->GetByteArray(v.num(), temp);
            for (uint32_t idx = 0; idx < temp.size(); ++idx) {
                const proto::binType& item = temp.at(idx);
                v.value().push_back(std::string((const char*)(item.first), item.second));
            }
        } else {
            _curMsg->GetStringArray(v.num(), v.value());
        }
    }

}
