#include "rapidjson/rapidjsonEncoder.h"
#include "thirdParty/rapidjson/writer.h"
#include "thirdParty/rapidjson/stringbuffer.h"


namespace struct2x {

    class writerWrapper {
        rapidjson::StringBuffer _str;
        rapidjson::Writer<rapidjson::StringBuffer> _writer;
    public:
        writerWrapper() :_writer(_str) {}

        rapidjson::Writer<rapidjson::StringBuffer>* operator->() { return &_writer; }

        const char* getString() const { return _str.GetString(); }
    };

    rapidjsonEncoder::rapidjsonEncoder() :_ptr(new writerWrapper), _writer(*_ptr) {

    }

    rapidjsonEncoder::~rapidjsonEncoder() {
        if (_ptr)
            delete _ptr;
    }

    bool rapidjsonEncoder::toString(std::string& str) {
        str = _writer.getString();
        return true;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, bool value, bool* pHas) {
        _writer->Key(sz);
        _writer->Bool(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, uint32_t value, bool* pHas) {
        _writer->Key(sz);
        _writer->Uint(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, int32_t value, bool* pHas) {
        _writer->Key(sz);
        _writer->Int(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, uint64_t value, bool* pHas) {
        _writer->Key(sz);
        _writer->Uint64(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, int64_t value, bool* pHas) {
        _writer->Key(sz);
        _writer->Int64(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, float value, bool* pHas) {
        _writer->Key(sz);
        _writer->Double(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, double value, bool* pHas) {
        _writer->Key(sz);
        _writer->Double(value);
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::string& value, bool* pHas) {
        _writer->Key(sz);
        _writer->String(value.c_str());
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<bool>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Bool(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<uint32_t>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Uint(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<int32_t>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Int(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<uint64_t>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Uint64(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<int64_t>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Int64(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<float>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Double(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<double>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->Double(value.at(i));
        }
        _writer->EndArray();
        return *this;
    }

    rapidjsonEncoder& rapidjsonEncoder::convert(const char* sz, const std::vector<std::string>& value, bool* pHas) {
        _writer->Key(sz);
        _writer->StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer->String(value.at(i).c_str());
        }
        _writer->EndArray();
        return *this;
    }

    void rapidjsonEncoder::StartObject(const char* sz) {
        if (sz)
            _writer->Key(sz);
        _writer->StartObject();
    }

    void rapidjsonEncoder::EndObject() {
        _writer->EndObject();
    }

    void rapidjsonEncoder::StartArray(const char* sz) {
        if (sz)
            _writer->Key(sz);
        _writer->StartArray();
    }

    void rapidjsonEncoder::EndArray() {
        _writer->EndArray();
    }

}
