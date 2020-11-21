#include "rapidjson/decoder.h"
//#include "thirdParty/rapidjson/reader.h"


namespace serialize {

    bool convertHandler::Null() {
        return true;
    }

    bool convertHandler::Bool(bool b) {
        if (_converter) {
            (*_converter)(_mgr->getStruct(), &b);
        }
        return true;
    }

    bool convertHandler::Int(int i) {
        if (_converter) {
            (*_converter)(_mgr->getStruct(), &i);
        }
        return true;
    }

    bool convertHandler::Uint(unsigned u) {
        if (_converter) {
            (*_converter)(_mgr->getStruct(), &u);
        }
        return true;
    }

    bool convertHandler::Int64(int64_t i) {
        if (_converter) {
            (*_converter)(_mgr->getStruct(), &i);
        }
        return true;
    }

    bool convertHandler::Uint64(uint64_t u) {
        if (_converter) {
            (*_converter)(_mgr->getStruct(), &u);
        }
        return true;
    }

    bool convertHandler::Double(double d) {
        if (_converter) {
            (*_converter)(_mgr->getStruct(), &d);
        }
        return true;
    }

    bool convertHandler::RawNumber(const char* str, unsigned length, bool copy) {
        return true;
    }

    bool convertHandler::String(const char* str, unsigned length, bool copy) {
        if (_converter) {
            std::string strValue(str, length);
            (*_converter)(_mgr->getStruct(), &strValue);
        }
        return true;
    }

    bool convertHandler::Key(const char* str, unsigned length, bool copy) {
        if (_mgr->isMap()) {
            const rapidJsonConverter* temp = _converter;
            _converter->setKey(str);
            rapidJsonConverterMgr* mgr = (rapidJsonConverterMgr*)_converter->getConvert(_stack.back()->getStruct());
            _converter = &mgr->find(str)->second;
            _converter->setLast(temp);
        } else {
            rapidJsonConverterMgr::const_iterator it = _mgr->find(str);
            if (it != _mgr->end()) {
                _converter = &it->second;
            } else {
                _converter = NULL;
            }
        }
        return true;
    }

    bool convertHandler::StartObject() {
        if (_converter) {
            _stackFunction.push_back(_converter);

            _stack.push_back(_mgr);
            _mgr = (rapidJsonConverterMgr*)_converter->getConvert(_mgr->getStruct());
        }
        return true;
    }

    bool convertHandler::EndObject(unsigned memberCount) {
        if (!_stack.empty()) {
            _mgr = _stack.back();
            _stack.erase(_stack.begin() + _stack.size() - 1);

            _converter = _stackFunction.back();
            _stackFunction.erase(_stackFunction.begin() + _stackFunction.size() - 1);
        }
        return true;
    }

    bool convertHandler::StartArray() {
        return true;
    }

    bool convertHandler::EndArray(unsigned elementCount) {
        if (!elementCount) {
            _converter->clear(_mgr->getStruct());
        }
        return true;
    }

    /*------------------------------------------------------------------------------*/

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, bool& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<bool>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, uint32_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<uint32_t>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, int32_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<int32_t>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, uint64_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<uint64_t>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, int64_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<int64_t>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, float& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<float>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, double& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<double>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::string& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bind<std::string>(&serialize::rapidjsonDecoder::convertValue, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<bool>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<bool>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<uint32_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<uint32_t>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<int32_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<int32_t>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<uint64_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<uint64_t>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<int64_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<int64_t>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<float>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<float>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<double>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<double>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

    rapidjsonDecoder& rapidjsonDecoder::convert(const char* sz, std::vector<std::string>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, rapidJsonConverter>(sz, rapidJsonConverterMgr::bindArray<std::string>(&serialize::rapidjsonDecoder::convertArray, offset, has)));
        return *this;
    }

}
