#include "json/decoder.h"


namespace serialize {

    JSONDecoder::JSONDecoder(const char* sz, uint32_t length)
        :_str(sz, length), _mgr(NULL) {
    }

    void JSONDecoder::decodeValue(const char* sz, bool& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<bool>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, uint32_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<uint32_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, int32_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<int32_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, uint64_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<uint64_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, int64_t& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<int64_t>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, float& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<float>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, double& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<double>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::string& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<std::string>(&serialize::JSONDecoder::convertValue, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<bool>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<bool>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<uint32_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<uint32_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<int32_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<int32_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<uint64_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<uint64_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<int64_t>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<int64_t>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<float>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<float>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<double>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<double>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<std::string>& value, bool* pHas) {
        size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
        size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
        _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<std::string>(&serialize::JSONDecoder::convertArray, offset, has)));
    }

    void JSONDecoder::convertValue(bool& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(int32_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(uint32_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(int64_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(uint64_t& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(float& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(double& value, const char* cValue, uint32_t length, bool* pHas) {
        custom::Handler::convert(value, cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertValue(std::string& value, const char* cValue, uint32_t length, bool* pHas) {
        value.append(cValue, length);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<bool>& value, const char* cValue, uint32_t length, bool* pHas) {
        bool v = false;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<int32_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        int32_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<uint32_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        uint32_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<int64_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        int64_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<uint64_t>& value, const char* cValue, uint32_t length, bool* pHas) {
        uint64_t v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<float>& value, const char* cValue, uint32_t length, bool* pHas) {
        float v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<double>& value, const char* cValue, uint32_t length, bool* pHas) {
        double v = 0;
        custom::Handler::convert(v, cValue, length);
        value.push_back(v);
        if (pHas) *pHas = true;
    }

    void JSONDecoder::convertArray(std::vector<std::string>& value, const char* cValue, uint32_t length, bool* pHas) {
        value.push_back(std::string(cValue, length));
        if (pHas) *pHas = true;
    }

}
