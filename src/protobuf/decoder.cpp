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

    void PBDecoder::decodeValue(serializeItem<bool>& v) {
        if (!_curMsg) return;

        uint64_t value = 0;
        if (_curMsg->GetVarInt(v.num, value)) {
            v.setValue((bool)value);
        }
    }

    void PBDecoder::decodeValue(serializeItem<int32_t>& v) {
        if (!_curMsg) return;

        uint64_t value = 0;
        if (v.type == TYPE_SVARINT) {
            if (_curMsg->GetSInt(v.num, value)) {
                v.setValue((int32_t)value);
            }
        } else if (v.type == TYPE_FIXED32) {
            uint32_t value = 0;
            if (_curMsg->GetFixedInt32(v.num, value)) {
                v.setValue((int32_t)value);
            }
        } else {
            if (_curMsg->GetVarInt(v.num, value))
                v.setValue((int32_t)value);
        }
    }

    void PBDecoder::decodeValue(serializeItem<uint32_t>& v) {
        if (!_curMsg) return;

        if (v.type == TYPE_FIXED32) {
            uint32_t value = 0;
            if (_curMsg->GetFixedInt32(v.num, value)) {
                v.setValue((uint32_t)value);
            }
        } else {
            uint64_t value = 0;
            if (_curMsg->GetVarInt(v.num, value))
                v.setValue((uint32_t)value);
        }
    }

    void PBDecoder::decodeValue(serializeItem<int64_t>& v) {
        if (!_curMsg) return;

        uint64_t value = 0;
        if (v.type == TYPE_SVARINT) {
            if (_curMsg->GetSInt(v.num, value))
                v.setValue((int64_t)value);
        } else {
            if (_curMsg->GetVarInt(v.num, value))
                v.setValue((int64_t)value);
        }
    }

    void PBDecoder::decodeValue(serializeItem<uint64_t>& v) {
        if (!_curMsg) return;

        uint64_t value = 0;
        if (v.type == TYPE_SVARINT) {
            if (_curMsg->GetSInt(v.num, value))
                v.setValue(value);
        } else {
            if (_curMsg->GetVarInt(v.num, value))
                v.setValue(value);
        }
    }

    void PBDecoder::decodeValue(serializeItem<float>& v) {
        if (!_curMsg) return;

        v.setHas(_curMsg->GetFloat(v.num, v.value));
    }

    void PBDecoder::decodeValue(serializeItem<double>& v) {
        if (!_curMsg) return;

        v.setHas(_curMsg->GetDouble(v.num, v.value));
    }

    void PBDecoder::decodeValue(serializeItem<std::string>& v) {
        if (!_curMsg) return;

        //TYPE_BYTES TYPE_STRING
        v.setHas(_curMsg->GetString(v.num, v.value));
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<bool> >& v) {
        if (!_curMsg) return;

        _curMsg->GetVarIntArray(v.num, v.value);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<int32_t> >& v) {
        if (!_curMsg) return;

        if (v.type == TYPE_SVARINT) {
            _curMsg->GetSIntArray<int32_t>(v.num, v.value);
        } else if (v.type == TYPE_FIXED32) {
            _curMsg->GetFixedInt32Array(v.num, v.value);
        } else {
            _curMsg->GetVarIntArray(v.num, v.value);
        }
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint32_t> >& v) {
        if (!_curMsg) return;

        if (v.type == TYPE_FIXED32) {
            _curMsg->GetFixedInt32Array(v.num, v.value);
        } else {
            _curMsg->GetVarIntArray(v.num, v.value);
        }
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<int64_t> >& v) {
        if (!_curMsg) return;

        if (v.type == TYPE_SVARINT) {
            _curMsg->GetSIntArray<int64_t>(v.num, v.value);
        } else {
            _curMsg->GetVarIntArray(v.num, v.value);
        }
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<uint64_t> >& v) {
        if (!_curMsg) return;

        _curMsg->GetVarIntArray(v.num, v.value);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<float> >& v) {
        if (!_curMsg) return;

        _curMsg->GetFloatArray(v.num, v.value);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<double> >& v) {
        if (!_curMsg) return;

        _curMsg->GetDoubleArray(v.num, v.value);
    }

    void PBDecoder::decodeRepaeted(serializeItem<std::vector<std::string> >& v) {
        if (!_curMsg) return;

        if (v.type == TYPE_BYTES) {
            std::vector<proto::binType> temp;
            _curMsg->GetByteArray(v.num, temp);
            for (uint32_t idx = 0; idx < temp.size(); ++idx) {
                const proto::binType& item = temp.at(idx);
                v.value.push_back(std::string((const char*)(item.first), item.second));
            }
        } else {
            _curMsg->GetStringArray(v.num, v.value);
        }
    }

}
