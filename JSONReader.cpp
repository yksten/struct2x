#include "JSONReader.h"
#include "thirdParty/cJSON.h"


JSONReader::JSONReader()
    : _root(cJSON_CreateObject())
    , _cur(_root)
{
}

JSONReader::~JSONReader()
{
    cJSON_Delete(_root);
}

JSONReader& JSONReader::convert(const char* sz, int value)
{
    cJSON_AddNumberToObject(_cur, sz, value);
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, float value)
{
    cJSON_AddNumberToObject(_cur, sz, value);
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, double value)
{
    cJSON_AddNumberToObject(_cur, sz, value);
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, unsigned int value)
{
    cJSON_AddNumberToObject(_cur, sz, value);
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, const char* value)
{
    cJSON_AddStringToObject(_cur, sz, value);
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, const std::string& value)
{
    cJSON_AddStringToObject(_cur, sz, value.c_str());
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, bool value)
{
    cJSON_AddBoolToObject(_cur, sz, value);
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, const std::vector<int>& value)
{
    cJSON_AddItemToObject(_cur, sz, cJSON_CreateIntArray(&value.front(), (int)value.size()));
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, const std::vector<float>& value)
{
    cJSON_AddItemToObject(_cur, sz, cJSON_CreateFloatArray(&value.front(), (int)value.size()));
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, const std::vector<double>& value)
{
    cJSON_AddItemToObject(_cur, sz, cJSON_CreateDoubleArray(&value.front(), (int)value.size()));
    return *this;
}

JSONReader& JSONReader::convert(const char* sz, const std::vector<std::string>& value)
{
    cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
    int count = (int)value.size();
    for (int i = 0; a && i < count; i++)
    {
        n = cJSON_CreateString(value[i].c_str());
        if (!i)
            a->child = n; 
        else
        {
            p->next = n;
            n->prev = p;
        }
        p = n;
    }
    cJSON_AddItemToObject(_cur, sz, a);
    return *this;
}

void JSONReader::toString(std::string& str)
{
    char *out = cJSON_Print(_root);
    str.append(out);
    free(out);
}

void JSONReader::createObject(const char* sz)
{
    cJSON *fmt = cJSON_CreateObject();
    cJSON_AddItemToObject(_cur, sz, fmt);
    _cur = fmt;
}

void JSONReader::createArray(const char* sz)
{
    cJSON *fmt = cJSON_CreateArray();
    cJSON_AddItemToObject(_cur, sz, fmt);
    _cur = fmt;
}

void JSONReader::addItemToArray()
{
    cJSON *fmt = cJSON_CreateObject();
    cJSON_AddItemToArray(_cur, fmt);
    _cur = fmt;
}

