#ifndef __BUFFER_INTERFACE_H__
#define __BUFFER_INTERFACE_H__

#include <cstdint>
#include <vector>
#include <string>
#include <assert.h>

class BufferInterface {
public:
    virtual ~BufferInterface() {}

    virtual const char* data() const = 0;
    virtual size_t size() const = 0;

    virtual void setValue(bool value) = 0;
    virtual void setValue(int8_t value) = 0;
    virtual void setValue(uint8_t value) = 0;
    virtual void setValue(int16_t value) = 0;
    virtual void setValue(uint16_t value) = 0;
    virtual void setValue(int32_t value) = 0;
    virtual void setValue(uint32_t value) = 0;
    virtual void setValue(int64_t value) = 0;
    virtual void setValue(uint64_t value) = 0;
    virtual void setValue(float value) = 0;
    virtual void setValue(double value) = 0;
    virtual void setValue(const std::string& value) = 0;
    virtual void setValue(const char* value) = 0;

    virtual void getValue(bool& value) = 0;
    virtual void getValue(int8_t& value) = 0;
    virtual void getValue(uint8_t& value) = 0;
    virtual void getValue(int16_t& value) = 0;
    virtual void getValue(uint16_t& value) = 0;
    virtual void getValue(int32_t& value) = 0;
    virtual void getValue(uint32_t& value) = 0;
    virtual void getValue(int64_t& value) = 0;
    virtual void getValue(uint64_t& value) = 0;
    virtual void getValue(float& value) = 0;
    virtual void getValue(double& value) = 0;
    virtual void getValue(std::string& value) = 0;
};

#endif