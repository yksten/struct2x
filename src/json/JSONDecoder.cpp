#include "json/JSONDecoder.h"
#include "thirdParty/cjson/cJSON.h"
#include <assert.h>

namespace struct2x {

    JSONDecoder::JSONDecoder(const char* sz)
        : _root(cJSON_Parse(sz))
        , _cur(_root) {
    }


    JSONDecoder::~JSONDecoder() {
        if (_root)
            cJSON_Delete(_root);
    }

    JSONDecoder& JSONDecoder::operator >>(std::vector<int32_t>& value) {
        if (!value.empty()) value.clear();
        if (_cur->type == cJSON_Array) {
            cJSON *c = _cur->child;
            while (c && (c->type == cJSON_Number)) {
                value.push_back(c->valueint);
                c = c->next;
            }
        }
        return *this;
    }

    JSONDecoder& JSONDecoder::operator >>(std::vector<float>& value) {
        if (!value.empty()) value.clear();
        if (_cur->type == cJSON_Array) {
            cJSON *c = _cur->child;
            while (c && (c->type == cJSON_Number)) {
                value.push_back((float)c->valuedouble);
                c = c->next;
            }
        }
        return *this;
    }

    JSONDecoder& JSONDecoder::operator >>(std::vector<double>& value) {
        if (!value.empty()) value.clear();
        if (_cur->type == cJSON_Array) {
            cJSON *c = _cur->child;
            while (c && (c->type == cJSON_Number)) {
                value.push_back(c->valuedouble);
                c = c->next;
            }
        }
        return *this;
    }

    JSONDecoder& JSONDecoder::operator >>(std::vector<std::string>& value) {
        if (!value.empty()) value.clear();
        cJSON *c = _cur->child;
        while (c && (c->type == cJSON_String)) {
            value.push_back(c->valuestring);
            c = c->next;
        }
        return *this;
    }

    void JSONDecoder::decodeValue(const char* sz, bool& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_False)
                value = false;
            else
                value = true;
            if (pHas) *pHas = true;
        }
    }

    void JSONDecoder::decodeValue(const char* sz, uint32_t& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_Number) {
                value = item->valueint;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, int32_t& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_Number) {
                value = item->valueint;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, uint64_t& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_Number) {
                value = item->valuedouble;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, int64_t& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_Number) {
                value = item->valuedouble;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, float& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_Number) {
                value = (float)item->valuedouble;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, double& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_Number) {
                value = item->valuedouble;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, std::string& value, bool* pHas) {
        if (cJSON * item = cJSON_GetObjectItem(_cur, sz)) {
            if (item->type == cJSON_String) {
                value = item->valuestring;
                if (pHas) *pHas = true;
            }
        }
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<bool>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back((bool)c->valueint);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<uint32_t>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back(c->valueint);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<int32_t>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back(c->valueint);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<uint64_t>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back((uint64_t)c->valuedouble);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<int64_t>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back((int64_t)c->valuedouble);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<float>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back((float)c->valuedouble);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<double>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_Number); c = c->next) {
                value.push_back(c->valuedouble);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    void JSONDecoder::decodeValue(const char* sz, std::vector<std::string>& value, bool* pHas) {
        if (!value.empty()) value.clear();
        cJSON* curItem = cur();
        if (getObject(sz) && (_cur->type == cJSON_Array)) {
            for (cJSON *c = _cur->child; c && (c->type == cJSON_String); c = c->next) {
                value.push_back(c->valuestring);
            }
            if (pHas) *pHas = true;
        }
        cur(curItem);
    }

    bool JSONDecoder::getObject(const char* sz) {
        if (cJSON *fmt = cJSON_GetObjectItem(_cur, sz)) {
            assert(fmt);
            _cur = fmt;
            return true;
        }
        return false;
    }

    int32_t JSONDecoder::getArraySize()const {
        return cJSON_GetArraySize(_cur);
    }

    void JSONDecoder::getArrayItem(int32_t i) {
        cJSON *fmt = cJSON_GetArrayItem(_cur, i);
        assert(fmt);
        _cur = fmt;
    }

    int32_t JSONDecoder::getMapSize() const {
        return getArraySize();
    }

    const char* JSONDecoder::getChildName(int32_t i) const {
        cJSON *fmt = _cur->child;
        for (int32_t index = 0; index < i; ++index) {
            fmt = fmt->next;
        }
        assert(fmt);
        return fmt->string;
    }

}