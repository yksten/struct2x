#include "cjson/encoder.h"
#include "thirdParty/cjson/cJSON.h"

namespace serialize {

    CJSONEncoder::CJSONEncoder()
        : _root(cJSON_CreateObject())
        , _cur(_root) {
    }

    CJSONEncoder::~CJSONEncoder() {
        cJSON_Delete(_root);
    }

    bool CJSONEncoder::toString(std::string& str, bool bUnformatted) {
        if (_root) {
            char *out = NULL;
            if (bUnformatted) {
                out = cJSON_PrintUnformatted(_root);
            } else {
                out = cJSON_Print(_root);
            }
            if (out) {
                str.append(out);
                free(out);
                return true;
            }
        }
        return false;
    }

    CJSONEncoder& CJSONEncoder::operator<<(const std::vector<int32_t>& value) {
        int32_t count = (int32_t)value.size();
        cJSON *n = 0, *p = 0, *a = _cur;
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateNumber(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        return *this;
    }

    CJSONEncoder& CJSONEncoder::operator<<(const std::vector<float>& value) {
        int32_t count = (int32_t)value.size();
        cJSON *n = 0, *p = 0, *a = _cur;
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateNumber(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        return *this;
    }

    CJSONEncoder& CJSONEncoder::operator<<(const std::vector<double>& value) {
        int32_t count = (int32_t)value.size();
        cJSON *n = 0, *p = 0, *a = _cur;
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateNumber(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        return *this;
    }

    CJSONEncoder& CJSONEncoder::operator<<(const std::vector<std::string>& value) {
        int32_t count = (int32_t)value.size();
        cJSON *n = 0, *p = 0, *a = _cur;
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateString(value[i].c_str());
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        return *this;
    }

    void CJSONEncoder::encodeValue(const char* sz, bool value) {
        cJSON_AddBoolToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, uint32_t value) {
        cJSON_AddNumberToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, int32_t value) {
        cJSON_AddNumberToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, uint64_t value) {
        cJSON_AddNumberToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, int64_t value) {
        cJSON_AddNumberToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, float value) {
        cJSON_AddNumberToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, double value) {
        cJSON_AddNumberToObject(_cur, sz, value);
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::string& value) {
        cJSON_AddStringToObject(_cur, sz, value.c_str());
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<bool>& value) {
        cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateBool(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        cJSON_AddItemToObject(_cur, sz, a);
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<uint32_t>& value) {
        cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateNumber(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        cJSON_AddItemToObject(_cur, sz, a);
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<int32_t>& value) {
        if (!value.empty()) {
            cJSON_AddItemToObject(_cur, sz, cJSON_CreateIntArray(&value.front(), (int32_t)value.size()));
        }
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<uint64_t>& value) {
        cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateNumber(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        cJSON_AddItemToObject(_cur, sz, a);
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<int64_t>& value) {
        cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateNumber(value[i]);
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        cJSON_AddItemToObject(_cur, sz, a);
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<float>& value) {
        if (!value.empty()) {
            cJSON_AddItemToObject(_cur, sz, cJSON_CreateFloatArray(&value.front(), (int32_t)value.size()));
        }
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<double>& value) {
        if (!value.empty()) {
            cJSON_AddItemToObject(_cur, sz, cJSON_CreateDoubleArray(&value.front(), (int32_t)value.size()));
        }
    }

    void CJSONEncoder::encodeValue(const char* sz, const std::vector<std::string>& value) {
        cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
        int32_t count = (int32_t)value.size();
        for (int32_t i = 0; a && i < count; i++) {
            n = cJSON_CreateString(value[i].c_str());
            if (!i)
                a->child = n;
            else {
                p->next = n;
                n->prev = p;
            }
            p = n;
        }
        cJSON_AddItemToObject(_cur, sz, a);
    }

    void CJSONEncoder::createObject(const char* sz) {
        cJSON *fmt = cJSON_CreateObject();
        cJSON_AddItemToObject(_cur, sz, fmt);
        _cur = fmt;
    }

    void CJSONEncoder::createArray(const char* sz) {
        cJSON *fmt = cJSON_CreateArray();
        cJSON_AddItemToObject(_cur, sz, fmt);
        _cur = fmt;
    }

    void CJSONEncoder::addItemToArray() {
        cJSON *fmt = cJSON_CreateObject();
        cJSON_AddItemToArray(_cur, fmt);
        _cur = fmt;
    }

    void CJSONEncoder::addArrayToArray() {
        cJSON *fmt = cJSON_CreateArray();
        cJSON_AddItemToArray(_cur, fmt);
        _cur = fmt;
    }

}
