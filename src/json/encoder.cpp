#include <struct2x/json/encoder.h>


namespace struct2x {

    JSONEncoder::JSONEncoder(std::string& str, bool formatted) : _writer(str, formatted) {
    }

    JSONEncoder::~JSONEncoder() {
    }

    void JSONEncoder::encodeValue(const char* sz, const bool& value) {
        _writer.Key(sz).Bool(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const uint32_t& value) {
        _writer.Key(sz).Uint64(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const int32_t& value) {
        _writer.Key(sz).Int64(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const uint64_t& value) {
        _writer.Key(sz).Uint64(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const int64_t& value) {
        _writer.Key(sz).Int64(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const float& value) {
        _writer.Key(sz).Double(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const double& value) {
        _writer.Key(sz).Double(value);
    }

    void JSONEncoder::encodeValue(const char* sz, const std::string& value) {
        _writer.Key(sz).String(value.c_str());
    }

    void JSONEncoder::encodeValue(const char* sz, const std::vector<bool>& value) {
        StartArray(sz);
        int32_t size = (int32_t)value.size();
        for (int32_t i = 0; i < size; ++i) {
            const bool item = value.at(i);
            encodeValue(NULL, item);
        }
        EndArray();
    }

    void JSONEncoder::StartObject(const char* sz) {
        _writer.Key(sz).StartObject();
    }

    void JSONEncoder::EndObject() {
        _writer.EndObject();
    }

    void JSONEncoder::StartArray(const char* sz) {
        _writer.Key(sz).StartArray();
    }

    void JSONEncoder::EndArray() {
        _writer.EndArray();
    }

}
