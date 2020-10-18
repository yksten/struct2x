#include "msgpack/encoder.h"

#   if defined(unix) || defined(__unix) || defined(__APPLE__) || defined(__OpenBSD__)
#       define _msgpack_be16(x) ntohs((uint16_t)x)
#   else
#       if defined(ntohs)
#           define _msgpack_be16(x) ntohs(x)
#       elif defined(_byteswap_ushort) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#           define _msgpack_be16(x) ((uint16_t)_byteswap_ushort((unsigned short)x))
#       else
#           define _msgpack_be16(x) ( \
                ((((uint16_t)x) <<  8) ) | \
                ((((uint16_t)x) >>  8) ) )
#        endif
#   endif

#   if defined(unix) || defined(__unix) || defined(__APPLE__) || defined(__OpenBSD__)
#       define _msgpack_be32(x) ntohl((uint32_t)x)
#   else
#       if defined(ntohl)
#           define _msgpack_be32(x) ntohl(x)
#       elif defined(_byteswap_ulong) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#           define _msgpack_be32(x) ((uint32_t)_byteswap_ulong((unsigned long)x))
#       else
#           define _msgpack_be32(x) \
                ( ((((uint32_t)x) << 24)               ) | \
                  ((((uint32_t)x) <<  8) & 0x00ff0000U ) | \
                  ((((uint32_t)x) >>  8) & 0x0000ff00U ) | \
                  ((((uint32_t)x) >> 24)               ) )
#       endif
#   endif

#   if defined(_byteswap_uint64) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#        define _msgpack_be64(x) (_byteswap_uint64(x))
#   elif defined(bswap_64)
#        define _msgpack_be64(x) bswap_64(x)
#   elif defined(__DARWIN_OSSwapInt64)
#        define _msgpack_be64(x) __DARWIN_OSSwapInt64(x)
#   else
#        define _msgpack_be64(x) \
             ( ((((uint64_t)x) << 56)                         ) | \
               ((((uint64_t)x) << 40) & 0x00ff000000000000ULL ) | \
               ((((uint64_t)x) << 24) & 0x0000ff0000000000ULL ) | \
               ((((uint64_t)x) <<  8) & 0x000000ff00000000ULL ) | \
               ((((uint64_t)x) >>  8) & 0x00000000ff000000ULL ) | \
               ((((uint64_t)x) >> 24) & 0x0000000000ff0000ULL ) | \
               ((((uint64_t)x) >> 40) & 0x000000000000ff00ULL ) | \
               ((((uint64_t)x) >> 56)                         ) )
#   endif

#define _msgpack_store16(to, num) \
    do { uint16_t val = _msgpack_be16(num); memcpy(to, &val, 2); } while(0)
#define _msgpack_store32(to, num) \
    do { uint32_t val = _msgpack_be32(num); memcpy(to, &val, 4); } while(0)
#define _msgpack_store64(to, num) \
    do { uint64_t val = _msgpack_be64(num); memcpy(to, &val, 8); } while(0)

#define TAKE8_8(d)  ((uint8_t*)&d)[0]
#define TAKE8_16(d) ((uint8_t*)&d)[0]
#define TAKE8_32(d) ((uint8_t*)&d)[0]
#define TAKE8_64(d) ((uint8_t*)&d)[0]

namespace struct2x {

    MPHelper::MPHelper(std::string& buf) :_buffer(buf), _flag(true), _fieldNum(0) {
    }

    MPHelper::~MPHelper() {
    }

    bool MPHelper::setKey(const char* sz) {
        if (_flag) {
            size_t l = strlen(sz);
            if (l < 32) {
                unsigned char d = 0xa0 | (uint8_t)l;
                _buffer.append((const char*)&TAKE8_8(d), 1);
            } else if (l < 256) {
                unsigned char buf[2];
                buf[0] = 0xd9; buf[1] = (uint8_t)l;
                _buffer.append((const char*)buf, 2);
            } else if (l < 65536) {
                unsigned char buf[3];
                buf[0] = 0xda; _msgpack_store16(&buf[1], (uint16_t)l);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdb; _msgpack_store32(&buf[1], (uint32_t)l);
                _buffer.append((const char*)buf, 5);
            }
            _buffer.append(sz, l);
        } else {
            ++_fieldNum;
        }
        return _flag;
    }

    void MPHelper::setValue(bool v) {
        if (v) {
            static const uint8_t dt = 0xc3;
            _buffer.append((const char*)&dt, 1);
        } else {
            static const uint8_t df = 0xc2;
            _buffer.append((const char*)&df, 1);
        }
    }

    void MPHelper::setValue(int32_t v) {
        do {
            if (v < -(1 << 5)) {
                if (v < -(1 << 15)) {
                    /* signed 32 */
                    unsigned char buf[5];
                    buf[0] = 0xd2; _msgpack_store32(&buf[1], (int32_t)v);
                    _buffer.append((const char*)buf, 5);
                } else if (v < -(1 << 7)) {
                    /* signed 16 */
                    unsigned char buf[3];
                    buf[0] = 0xd1; _msgpack_store16(&buf[1], (int16_t)v);
                    _buffer.append((const char*)buf, 3);
                } else {
                    /* signed 8 */
                    unsigned char buf[2] = { 0xd0, TAKE8_32(v) };
                    _buffer.append((const char*)buf, 2);
                }
            } else if (v < (1 << 7)) {
                /* fixnum */
                _buffer.append((const char*)&TAKE8_32(v), 1);
            } else {
                if (v < (1 << 8)) {
                    /* unsigned 8 */
                    unsigned char buf[2] = { 0xcc, TAKE8_32(v) };
                    _buffer.append((const char*)buf, 2);
                } else if (v < (1 << 16)) {
                    /* unsigned 16 */
                    unsigned char buf[3];
                    buf[0] = 0xcd; _msgpack_store16(&buf[1], (uint16_t)v);
                    _buffer.append((const char*)buf, 3);
                } else {
                    /* unsigned 32 */
                    unsigned char buf[5];
                    buf[0] = 0xce; _msgpack_store32(&buf[1], (uint32_t)v);
                    _buffer.append((const char*)buf, 5);
                }
            }
        } while (0);
    }

    void MPHelper::setValue(uint32_t v) {
    }

    void MPHelper::setValue(int64_t v) {
        do {
            if (v < -(1LL << 5)) {
                if (v < -(1LL << 15)) {
                    if (v < -(1LL << 31)) {
                        /* signed 64 */
                        unsigned char buf[9];
                        buf[0] = 0xd3; _msgpack_store64(&buf[1], v);
                        _buffer.append((const char*)buf, 9);
                    } else {
                        /* signed 32 */
                        unsigned char buf[5];
                        buf[0] = 0xd2; _msgpack_store32(&buf[1], (int32_t)v);
                        _buffer.append((const char*)buf, 5);
                    }
                } else {
                    if (v < -(1 << 7)) {
                        /* signed 16 */
                        unsigned char buf[3];
                        buf[0] = 0xd1; _msgpack_store16(&buf[1], (int16_t)v);
                        _buffer.append((const char*)buf, 3);
                    } else {
                        /* signed 8 */
                        unsigned char buf[2] = { 0xd0, TAKE8_64(v) };
                        _buffer.append((const char*)buf, 2);
                    }
                }
            } else if (v < (1 << 7)) {
                /* fixnum */
                _buffer.append((const char*)&TAKE8_64(v), 1);
            } else {
                if (v < (1LL << 16)) {
                    if (v < (1 << 8)) {
                        /* unsigned 8 */
                        unsigned char buf[2] = { 0xcc, TAKE8_64(v) };
                        _buffer.append((const char*)buf, 2);
                    } else {
                        /* unsigned 16 */
                        unsigned char buf[3];
                        buf[0] = 0xcd; _msgpack_store16(&buf[1], (uint16_t)v);
                        _buffer.append((const char*)buf, 3);
                    }
                } else {
                    if (v < (1LL << 32)) {
                        /* unsigned 32 */
                        unsigned char buf[5];
                        buf[0] = 0xce; _msgpack_store32(&buf[1], (uint32_t)v);
                        _buffer.append((const char*)buf, 5);
                    } else {
                        /* unsigned 64 */
                        unsigned char buf[9];
                        buf[0] = 0xcf; _msgpack_store64(&buf[1], v);
                        _buffer.append((const char*)buf, 9);
                    }
                }
            }
        } while (0);
    }

    void MPHelper::setValue(uint64_t v) {
    }

    void MPHelper::setValue(float v) {
        unsigned char buf[5];
        union { float f; uint32_t i; } mem;
        mem.f = v;
        buf[0] = 0xca; _msgpack_store32(&buf[1], mem.i);
        _buffer.append((const char*)buf, 5);
    }

    void MPHelper::setValue(double v) {
        unsigned char buf[9];
        union { double f; uint64_t i; } mem;
        mem.f = v;
        buf[0] = 0xcb;
#if defined(TARGET_OS_IPHONE)
        // ok
#elif defined(__arm__) && !(__ARM_EABI__) // arm-oabi
        // https://github.com/msgpack/msgpack-perl/pull/1
        mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
        _msgpack_store64(&buf[1], mem.i);
        _buffer.append((const char*)buf, 9);
    }

    void MPHelper::setValue(const std::string& v) {
        if (!checkEmpty(v)) {
            size_t l = v.size();
            if (l < 32) {
                unsigned char d = 0xa0 | (uint8_t)l;
                _buffer.append((const char*)&TAKE8_8(d), 1);
            } else if (l < 256) {
                unsigned char buf[2];
                buf[0] = 0xd9; buf[1] = (uint8_t)l;
                _buffer.append((const char*)buf, 2);
            } else if (l < 65536) {
                unsigned char buf[3];
                buf[0] = 0xda; _msgpack_store16(&buf[1], (uint16_t)l);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdb; _msgpack_store32(&buf[1], (uint32_t)l);
                _buffer.append((const char*)buf, 5);
            }
            _buffer.append(v.c_str(), l);
        }
    }

    void MPHelper::setMapSize(size_t n) {
        if (n < 16) {
            unsigned char d = 0x80 | (uint8_t)n;
            _buffer.append((const char*)&TAKE8_8(d), 1);
        } else if (n < 65536) {
            unsigned char buf[3];
            buf[0] = 0xde; _msgpack_store16(&buf[1], (uint16_t)n);
            _buffer.append((const char*)buf, 3);
        } else {
            unsigned char buf[5];
            buf[0] = 0xdf; _msgpack_store32(&buf[1], (uint32_t)n);
            _buffer.append((const char*)buf, 5);
        }
    }

    void MPHelper::setArraySize(size_t n) {
        if (n < 16) {
            unsigned char d = 0x90 | (uint8_t)n;
            _buffer.append((const char*)&d, 1);
        } else if (n < 65536) {
            unsigned char buf[3];
            buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
            _buffer.append((const char*)buf, 3);
        } else {
            unsigned char buf[5];
            buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
            _buffer.append((const char*)buf, 5);
        }
    }

    //-------------------------------------------------------------------------------------------------------//
    MPEncoder::MPEncoder(std::string& buf):_buffer(buf), _helper(_buffer) {
    }

    MPEncoder::~MPEncoder() {
    }

    void MPEncoder::setValue(const char* sz, bool value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, uint32_t value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, int32_t value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, uint64_t value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, int64_t value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, float value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, double value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, const std::string& value) {
        if (_helper.setKey(sz)) {
            _helper.setValue(value);
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<bool>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<uint32_t>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<int32_t>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<uint64_t>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<int64_t>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<float>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<double>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

    void MPEncoder::setValue(const char* sz, const std::vector<std::string>& value) {
        if (_helper.setKey(sz) && !_helper.checkEmpty(value)) {
            size_t n = value.size();
            if (n < 16) {
                unsigned char d = 0x90 | (uint8_t)n;
                _buffer.append((const char*)&d, 1);
            } else if (n < 65536) {
                unsigned char buf[3];
                buf[0] = 0xdc; _msgpack_store16(&buf[1], (uint16_t)n);
                _buffer.append((const char*)buf, 3);
            } else {
                unsigned char buf[5];
                buf[0] = 0xdd; _msgpack_store32(&buf[1], (uint32_t)n);
                _buffer.append((const char*)buf, 5);
            }
            for (uint32_t idx = 0; idx < n; ++idx) {
                _helper.setValue(value[idx]);
            }
        }
    }

}
