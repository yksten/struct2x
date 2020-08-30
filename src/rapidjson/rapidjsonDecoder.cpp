#include "rapidjson/rapidjsonDecoder.h"
//#include "thirdParty/rapidjson/reader.h"


namespace struct2x {

    bool convertHandler::Null() {
        return true;
    }

    bool convertHandler::Bool(bool b) {
        return true;
    }

    bool convertHandler::Int(int i) {
        if (_converter)
            (*_converter)(&i);
        return true;
    }

    bool convertHandler::Uint(unsigned u) {
        if (_converter)
            (*_converter)(&u);
        return true;
    }

    bool convertHandler::Int64(int64_t i) {
        return true;
    }

    bool convertHandler::Uint64(uint64_t u) {
        return true;
    }

    bool convertHandler::Double(double d) {
        return true;
    }

    bool convertHandler::RawNumber(const char* str, unsigned length, bool copy) {
        return true;
    }

    bool convertHandler::String(const char* str, unsigned length, bool copy) {
        return true;
    }

    bool convertHandler::StartObject() {
        return true;
    }

    bool convertHandler::Key(const char* str, unsigned length, bool copy) {
        std::map<std::string, converter>::const_iterator it = _map.find(str);
        if (it != _map.end()) {
            _converter = &it->second;
        }

        return true;
    }

    bool convertHandler::EndObject(unsigned memberCount) {
        return true;
    }

    bool convertHandler::StartArray() {
        return true;
    }

    bool convertHandler::EndArray(unsigned elementCount) {
        return true;
    }

}
