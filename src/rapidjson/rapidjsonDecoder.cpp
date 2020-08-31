#include "rapidjson/rapidjsonDecoder.h"
//#include "thirdParty/rapidjson/reader.h"


namespace struct2x {

    bool convertHandler::Null() {
        return true;
    }

    bool convertHandler::Bool(bool b) {
        if (_converter) {
            (*_converter)(&b);
            return true;
        }
        return false;
    }

    bool convertHandler::Int(int i) {
        if (_converter) {
            (*_converter)(&i);
            return true;
        }
        return false;
    }

    bool convertHandler::Uint(unsigned u) {
        if (_converter) {
            (*_converter)(&u);
            return true;
        }
        return false;
    }

    bool convertHandler::Int64(int64_t i) {
        if (_converter) {
            (*_converter)(&i);
            return true;
        }
        return false;
    }

    bool convertHandler::Uint64(uint64_t u) {
        if (_converter) {
            (*_converter)(&u);
            return true;
        }
        return false;
    }

    bool convertHandler::Double(double d) {
        if (_converter) {
            (*_converter)(&d);
            return true;
        }
        return false;
    }

    bool convertHandler::RawNumber(const char* str, unsigned length, bool copy) {
        return true;
    }

    bool convertHandler::String(const char* str, unsigned length, bool copy) {
        if (_converter) {
            std::string strValue(str, length);
            (*_converter)(&strValue);
            return true;
        }
        return false;
    }

    bool convertHandler::StartObject() {
        return true;
    }

    bool convertHandler::Key(const char* str, unsigned length, bool copy) {
        std::map<std::string, converter>::const_iterator it = _map.find(str);
        if (it != _map.end()) {
            _converter = &it->second;
            return true;
        }
        return false;
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

    /*------------------------------------------------------------------------------*/

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, bool& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, uint32_t& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, int32_t& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, uint64_t& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, int64_t& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, float& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, double& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::string& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertValue, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<bool>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<uint32_t>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<int32_t>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<uint64_t>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<int64_t>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<float>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<double>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<std::string>& value, bool* pHas) {
        _map.insert(std::pair<std::string, converter>(sz, bind(&struct2x::rapidjsonDecoder::convertArray, value, pHas)));
        return *this;
    }

}
