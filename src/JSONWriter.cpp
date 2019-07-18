#include "JSONWriter.h"
#include "thirdParty/cJSON.h"
#include <assert.h>


JSONWriter::JSONWriter(const char* sz)
    : _root(cJSON_Parse(sz))
    , _cur(_root)
{
}


JSONWriter::~JSONWriter()
{
    if (_root)
        cJSON_Delete(_root);
}

JSONWriter& JSONWriter::convert(const char* sz, int& value)
{
    if (cJSON * item = cJSON_GetObjectItem(_cur, sz))
        value = item->valueint;
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, float& value)
{
    if (cJSON * item = cJSON_GetObjectItem(_cur, sz))
        value = item->valuedouble;
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, double& value)
{
    if (cJSON * item = cJSON_GetObjectItem(_cur, sz))
        value = item->valuedouble;
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, unsigned int& value)
{
    if (cJSON * item = cJSON_GetObjectItem(_cur, sz))
        value = item->valueint;
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, std::string& value)
{
    if (cJSON * item = cJSON_GetObjectItem(_cur, sz))
        value = item->valuestring;
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, bool& value)
{
    if (cJSON * item = cJSON_GetObjectItem(_cur, sz))
        value = item->valueint;
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, std::vector<int>& value)
{
    cJSON* curItem = cur();
    getObject(sz);
    cJSON *c = _cur->child;
    while (c)
    {
        value.push_back(c->valueint);
        c = c->next;
    }
    cur(curItem);
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, std::vector<float>& value)
{
    cJSON* curItem = cur();
    getObject(sz);
    cJSON *c = _cur->child;
    while (c)
    {
        value.push_back(c->valuedouble);
        c = c->next;
    }
    cur(curItem);
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, std::vector<double>& value)
{
    cJSON* curItem = cur();
    getObject(sz);
    cJSON *c = _cur->child;
    while (c)
    {
        value.push_back(c->valuedouble);
        c = c->next;
    }
    cur(curItem);
    return *this;
}

JSONWriter& JSONWriter::convert(const char* sz, std::vector<std::string>& value)
{
    cJSON* curItem = cur();
    getObject(sz);
    cJSON *c = _cur->child;
    while (c)
    {
        value.push_back(c->valuestring);
        c = c->next;
    }
    cur(curItem);
    return *this;
}

void JSONWriter::getObject(const char* sz)
{
    cJSON *fmt = cJSON_GetObjectItem(_cur, sz);
    assert(fmt);
    _cur = fmt;
}

int JSONWriter::getArraySize()const
{
    return cJSON_GetArraySize(_cur);
}

void JSONWriter::getArrayItem(int i)
{
    cJSON *fmt = cJSON_GetArrayItem(_cur, i);
    assert(fmt);
    _cur = fmt;
}

int JSONWriter::getMapSize() const
{
    return getArraySize();
}

const char* JSONWriter::getChildName(int i) const
{
    cJSON *fmt = _cur->child;
    for(int index=0; index<i; ++index)
    {
        fmt = fmt->next;
    }
    assert(fmt);
    return fmt->string;
}
