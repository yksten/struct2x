#include "json/encoder.h"
#include <assert.h>


namespace serialize {

    JSONEncoder::JSONEncoder(std::string& str) : _writer(str) {
    }

    JSONEncoder::~JSONEncoder() {
    }

    void JSONEncoder::encodeValue(const char* sz, bool value, bool* pHas) {
        _writer.Key(sz);
        _writer.Bool(value);
    }

    void JSONEncoder::encodeValue(const char* sz, uint32_t value, bool* pHas) {
        _writer.Key(sz);
        _writer.Uint(value);
    }

    void JSONEncoder::encodeValue(const char* sz, int32_t value, bool* pHas) {
        _writer.Key(sz);
        _writer.Int(value);
    }

    void JSONEncoder::encodeValue(const char* sz, uint64_t value, bool* pHas) {
        _writer.Key(sz);
        _writer.Uint64(value);
    }

    void JSONEncoder::encodeValue(const char* sz, int64_t value, bool* pHas) {
        _writer.Key(sz);
        _writer.Int64(value);
    }

    void JSONEncoder::encodeValue(const char* sz, float value, bool* pHas) {
        _writer.Key(sz);
        _writer.Double(value);
    }

    void JSONEncoder::encodeValue(const char* sz, double value, bool* pHas) {
        _writer.Key(sz);
        _writer.Double(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const std::string& value, bool* pHas) {
        _writer.Key(sz);
        _writer.String(value.c_str());
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<bool>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Bool(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<uint32_t>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Uint(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<int32_t>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Int(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<uint64_t>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Uint64(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<int64_t>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Int64(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<float>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Double(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<double>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.Double(value.at(i));
        }
        _writer.EndArray();
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<std::string>& value, bool* pHas) {
        _writer.Key(sz);
        _writer.StartArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; i < count; i++) {
            _writer.String(value.at(i).c_str());
        }
        _writer.EndArray();
    }

    void JSONEncoder::StartObject(const char* sz) {
        if (sz)
            _writer.Key(sz);
        _writer.StartObject();
    }

    void JSONEncoder::EndObject() {
        _writer.EndObject();
    }

    void JSONEncoder::StartArray(const char* sz) {
        if (sz)
            _writer.Key(sz);
        _writer.StartArray();
    }

    void JSONEncoder::EndArray() {
        _writer.EndArray();
    }

}
