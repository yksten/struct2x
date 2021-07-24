#ifndef __SERIALIZE_MACRO_H__
#define __SERIALIZE_MACRO_H__

#include <stdlib.h>
#ifdef _MSC_VER
#ifdef EXPORTAPI 
#define EXPORTAPI _declspec(dllimport)
#else 
#define EXPORTAPI _declspec(dllexport)
#endif
#else
#define EXPORTAPI
#endif

#ifdef _MSC_VER_ // for MSVC
#define FORCEINLINE __forceinline
#elif defined __GNUC__ // for gcc on Linux/Apple OS X
#define FORCEINLINE __inline__ __attribute__((always_inline))
#else
#define FORCEINLINE
#endif

#include <stdint.h>
#include <string>
#include <vector>


#define SERIALIZATION_2(num, value)                 serialize::makeItem(#value, num, value)
#define SERIALIZATION_3(num, value, typeOrHas)      serialize::makeItem(#value, num, value, typeOrHas)
#define SERIALIZATION_4(num, value, type, has)      serialize::makeItem(#value, num, value, type, has)

#define EXPAND(args)                                args
#define MAKE_TAG_COUNT(TAG, _4, _3,_2,_1,N,...)     TAG##N
#define SERIALIZATION(...)                          EXPAND(MAKE_TAG_COUNT(SERIALIZATION, __VA_ARGS__, _4, _3,_2,_1) (__VA_ARGS__))

namespace serialize {

    const uint32_t BITNUM = 16;

    enum {
        TYPE_VARINT = 0,    // int32,int64,uint32,uint64,bool,enum
        TYPE_SVARINT = 1,   // sint32,sin64
        TYPE_FIXED32 = 2,   // fixed32,sfixed32
        TYPE_FIXED64 = 3,   // fixed64,sfixed64
        TYPE_BYTES = 4,     // bytes
        TYPE_PACK = 5,      // repaeted [pack=true]
    };

    template<typename VALUE>
    struct serializeItem {
        serializeItem(const char* sz, uint32_t n, VALUE& v, bool* b) : name(sz), num(n), value(v), type(TYPE_VARINT), bHas(b){}
        serializeItem(const char* sz, uint32_t n, VALUE& v, uint32_t t, bool* b) : name(sz), num(n), value(v), type(t), bHas(b) {}

        const char* name;
        const uint32_t num;
        VALUE& value;
        const uint32_t type;
        bool* bHas;

        void setHas(bool b) {   //proto2 has
            if (bHas)
                *bHas = b;
        }
        void setValue(const VALUE& v) {
            value = v;
            setHas(true);
        }
    };

    template<typename VALUE>
    inline serializeItem<VALUE> makeItem(const char* sz, uint32_t num, VALUE& value, bool* b = NULL) {
        return serializeItem<VALUE>(sz, num, value, b);
    }

    template<typename VALUE>
    inline serializeItem<VALUE> makeItem(const char* sz, uint32_t num, VALUE& value, int32_t type, bool* b = NULL) {
        return serializeItem<VALUE>(sz, num, value, type, b);
    }

    namespace internal {

        // These are defined in:
        // https://developers.google.com/protocol-buffers/docs/encoding
        enum WireType {
            WT_VARINT = 0,                // int32,int64,uint32,uint64,sint32,sin64,bool,enum
            WT_64BIT = 1,                 // fixed64,sfixed64,double
            WT_LENGTH_DELIMITED = 2,      // string,bytes,embedded messages,packed repeated fields
            WT_GROUP_START = 3,           // Groups(deprecated)
            WT_GROUP_END = 4,             // Groups(deprecated)
            WT_32BIT = 5,                 // fixed32,sfixed32,float
        };

        template<typename From, typename To>
        class is_convertible {
            typedef char one;
            typedef int  two;

            template<typename To1>
            static To1& create();

            template<typename To1>
            static one test(To1);

            template<typename>
            static two test(...);
        public:
            static const bool value = (sizeof(test<To>(create<From>())) == sizeof(one));
        };

        template <class T> struct is_integral { static const bool value = false; };
        template <> struct is_integral<bool> { static const bool value = true; };
        template <> struct is_integral<int32_t> { static const bool value = true; };
        template <> struct is_integral<uint32_t> { static const bool value = true; };
        template <> struct is_integral<int64_t> { static const bool value = true; };
        template <> struct is_integral<uint64_t> { static const bool value = true; };
        template <> struct is_integral<float> { static const bool value = true; };
        template <> struct is_integral<double> { static const bool value = true; };

        template <class T>
        struct is_enum {
            static const bool value = is_convertible<T, int32_t>::value & !is_integral<T>::value;
        };

        template <typename T, bool isEnum = is_enum<T>::value> struct isMessage { enum { WRITE_TYPE = WT_LENGTH_DELIMITED }; };
        template <typename T> struct isMessage<T, true> { enum { WRITE_TYPE = WT_VARINT }; };
        template<> struct isMessage<std::string> { enum { WRITE_TYPE = WT_LENGTH_DELIMITED }; };
        template<> struct isMessage<bool> { enum { WRITE_TYPE = WT_VARINT }; };
        template<> struct isMessage<int32_t> { enum { WRITE_TYPE = WT_VARINT }; };
        template<> struct isMessage<uint32_t> { enum { WRITE_TYPE = WT_VARINT }; };
        template<> struct isMessage<int64_t> { enum { WRITE_TYPE = WT_VARINT }; };
        template<> struct isMessage<uint64_t> { enum { WRITE_TYPE = WT_VARINT }; };
        template<> struct isMessage<float> { enum { WRITE_TYPE = WT_32BIT }; };
        template<> struct isMessage<double> { enum { WRITE_TYPE = WT_64BIT }; };

        template<class T, class C>
        FORCEINLINE void serialize(T& t, C& c) {
            c.serialize(t);
        }

        template<class T, class C>
        FORCEINLINE void serializeWrapper(T& t, C& c) {
            serialize(t, c);
        }

        template<typename T, bool isDeserialize = false, bool isEnum = is_enum<T>::value> struct TypeTraits { typedef T Type; };
        template<typename T, bool isDeserialize, bool isEnum> struct TypeTraits<std::vector<T>, isDeserialize, isEnum> { typedef std::vector<T> Type; };
        template<typename T> struct TypeTraits<T, false, true> { typedef int32_t Type; };
        template<typename T> struct TypeTraits<T, true, true> { typedef int32_t Type; };
        template<> struct TypeTraits<float, false, false> { typedef double Type; };
        template<> struct TypeTraits<int32_t, false, false> { typedef int64_t Type; };
        template<> struct TypeTraits<uint32_t, false, false> { typedef uint64_t Type; };

        namespace STOT {
            template<typename T> struct type {};
            const int32_t _RTBUFSIZE = 128;
            static char szTransbuf[_RTBUFSIZE] = { 0 };
            //bool
            template<> struct type<bool> {
                static inline bool strto(const char* str) { return atoi(str) != 0; }
                static inline const char* tostr(bool v) { return v ? "1" : "0"; }
            };
            //int32_t
            template<> struct type<int32_t> {
                static inline int32_t strto(const char* str) { return (int32_t)atoi(str); }
                static inline const char* tostr(int32_t v) { sprintf(szTransbuf, "%d", v); return szTransbuf; }
            };
            //uint32_t
            template<> struct type<uint32_t> {
                static inline uint32_t strto(const char* str) { return (uint32_t)atoi(str); }
                static inline const char* tostr(uint32_t v) { sprintf(szTransbuf, "%u", v); return szTransbuf; }
            };
            //int64_t
            template<> struct type<int64_t> {
                static inline int64_t strto(const char* str) { return atoll(str); }
                static inline const char* tostr(int64_t v) { sprintf(szTransbuf, "%lld", (long long)v); return szTransbuf; }
            };
            //uint64_t
            template<> struct type<uint64_t> {
                static inline uint64_t strto(const char* str) { return (uint64_t)atoll(str); }
                static inline const char* tostr(uint64_t v) { sprintf(szTransbuf, "%llu", (long long)v); return szTransbuf; }
            };
            //std::string
            template<> struct type<std::string> {
                static inline const std::string& strto(const std::string& str) { return str; }
                static inline const char* tostr(const std::string& v) { return v.c_str(); }
            };

        }//namespace STOT

    }

}

#define EXPAND(args) args

#define SERIALIZE_0(p) convert(#p, p)
#define SERIALIZE_1(p) SERIALIZE_0(p)
#define SERIALIZE_2(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_1(__VA_ARGS__))
#define SERIALIZE_3(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_2(__VA_ARGS__))
#define SERIALIZE_4(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_3(__VA_ARGS__))
#define SERIALIZE_5(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_4(__VA_ARGS__))
#define SERIALIZE_6(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_5(__VA_ARGS__))
#define SERIALIZE_7(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_6(__VA_ARGS__))
#define SERIALIZE_8(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_7(__VA_ARGS__))
#define SERIALIZE_9(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_8(__VA_ARGS__))
#define SERIALIZE_10(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_9(__VA_ARGS__))
#define SERIALIZE_11(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_10(__VA_ARGS__))
#define SERIALIZE_12(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_11(__VA_ARGS__))
#define SERIALIZE_13(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_12(__VA_ARGS__))
#define SERIALIZE_14(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_13(__VA_ARGS__))
#define SERIALIZE_15(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_14(__VA_ARGS__))
#define SERIALIZE_16(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_15(__VA_ARGS__))
#define SERIALIZE_17(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_16(__VA_ARGS__))
#define SERIALIZE_18(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_17(__VA_ARGS__))
#define SERIALIZE_19(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_18(__VA_ARGS__))
#define SERIALIZE_20(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_19(__VA_ARGS__))
#define SERIALIZE_21(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_20(__VA_ARGS__))
#define SERIALIZE_22(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_21(__VA_ARGS__))
#define SERIALIZE_23(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_22(__VA_ARGS__))
#define SERIALIZE_24(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_23(__VA_ARGS__))
#define SERIALIZE_25(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_24(__VA_ARGS__))
#define SERIALIZE_26(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_25(__VA_ARGS__))
#define SERIALIZE_27(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_26(__VA_ARGS__))
#define SERIALIZE_28(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_27(__VA_ARGS__))
#define SERIALIZE_29(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_28(__VA_ARGS__))
#define SERIALIZE_30(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_29(__VA_ARGS__))
#define SERIALIZE_31(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_30(__VA_ARGS__))
#define SERIALIZE_32(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_31(__VA_ARGS__))
#define SERIALIZE_33(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_32(__VA_ARGS__))
#define SERIALIZE_34(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_33(__VA_ARGS__))
#define SERIALIZE_35(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_34(__VA_ARGS__))
#define SERIALIZE_36(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_35(__VA_ARGS__))
#define SERIALIZE_37(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_36(__VA_ARGS__))
#define SERIALIZE_38(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_37(__VA_ARGS__))
#define SERIALIZE_39(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_38(__VA_ARGS__))
#define SERIALIZE_40(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_39(__VA_ARGS__))
#define SERIALIZE_41(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_40(__VA_ARGS__))
#define SERIALIZE_42(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_41(__VA_ARGS__))
#define SERIALIZE_43(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_42(__VA_ARGS__))
#define SERIALIZE_44(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_43(__VA_ARGS__))
#define SERIALIZE_45(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_44(__VA_ARGS__))
#define SERIALIZE_46(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_45(__VA_ARGS__))
#define SERIALIZE_47(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_46(__VA_ARGS__))
#define SERIALIZE_48(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_47(__VA_ARGS__))
#define SERIALIZE_49(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_48(__VA_ARGS__))
#define SERIALIZE_50(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_49(__VA_ARGS__))
#define SERIALIZE_51(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_50(__VA_ARGS__))
#define SERIALIZE_52(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_51(__VA_ARGS__))
#define SERIALIZE_53(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_52(__VA_ARGS__))
#define SERIALIZE_54(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_53(__VA_ARGS__))
#define SERIALIZE_55(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_54(__VA_ARGS__))
#define SERIALIZE_56(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_55(__VA_ARGS__))
#define SERIALIZE_57(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_56(__VA_ARGS__))
#define SERIALIZE_58(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_57(__VA_ARGS__))
#define SERIALIZE_59(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_58(__VA_ARGS__))
#define SERIALIZE_60(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_59(__VA_ARGS__))
#define SERIALIZE_61(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_60(__VA_ARGS__))
#define SERIALIZE_62(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_61(__VA_ARGS__))
#define SERIALIZE_63(p, ...) SERIALIZE_0(p) . EXPAND(SERIALIZE_62(__VA_ARGS__))


#define MAKE_CALL(t, f) t . f

#define PARAMS_COUNT(TAG, _63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,\
                        _50,_49,_48,_47,_46,_45,_44,_43,_42,_41,\
                        _40,_39,_38,_37,_36,_35,_34,_33,_32,_31,\
                        _30,_29,_28,_27,_26,_25,_24,_23,_22,_21,\
                        _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,\
                        _10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N

#define SERIALIZE(t, ...) MAKE_CALL(t, EXPAND(PARAMS_COUNT(SERIALIZE, __VA_ARGS__, \
                        _63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,\
                        _50,_49,_48,_47,_46,_45,_44,_43,_42,_41,\
                        _40,_39,_38,_37,_36,_35,_34,_33,_32,_31,\
                        _30,_29,_28,_27,_26,_25,_24,_23,_22,_21,\
                        _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,\
                        _10,_9,_8,_7,_6,_5,_4,_3,_2,_1))(__VA_ARGS__))


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NI_EXPAND(args) args

#define NISERIALIZE_0(c, p) convert(#p, c.p)
#define NISERIALIZE_1(c, p) NISERIALIZE_0(c, p)
#define NISERIALIZE_2(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_1(c, __VA_ARGS__))
#define NISERIALIZE_3(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_2(c, __VA_ARGS__))
#define NISERIALIZE_4(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_3(c, __VA_ARGS__))
#define NISERIALIZE_5(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_4(c, __VA_ARGS__))
#define NISERIALIZE_6(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_5(c, __VA_ARGS__))
#define NISERIALIZE_7(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_6(c, __VA_ARGS__))
#define NISERIALIZE_8(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_7(c, __VA_ARGS__))
#define NISERIALIZE_9(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_8(c, __VA_ARGS__))
#define NISERIALIZE_10(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_9(c, __VA_ARGS__))
#define NISERIALIZE_11(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_10(c, __VA_ARGS__))
#define NISERIALIZE_12(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_11(c, __VA_ARGS__))
#define NISERIALIZE_13(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_12(c, __VA_ARGS__))
#define NISERIALIZE_14(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_13(c, __VA_ARGS__))
#define NISERIALIZE_15(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_14(c, __VA_ARGS__))
#define NISERIALIZE_16(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_15(c, __VA_ARGS__))
#define NISERIALIZE_17(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_16(c, __VA_ARGS__))
#define NISERIALIZE_18(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_17(c, __VA_ARGS__))
#define NISERIALIZE_19(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_18(c, __VA_ARGS__))
#define NISERIALIZE_20(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_19(c, __VA_ARGS__))
#define NISERIALIZE_21(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_20(c, __VA_ARGS__))
#define NISERIALIZE_22(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_21(c, __VA_ARGS__))
#define NISERIALIZE_23(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_22(c, __VA_ARGS__))
#define NISERIALIZE_24(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_23(c, __VA_ARGS__))
#define NISERIALIZE_25(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_24(c, __VA_ARGS__))
#define NISERIALIZE_26(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_25(c, __VA_ARGS__))
#define NISERIALIZE_27(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_26(c, __VA_ARGS__))
#define NISERIALIZE_28(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_27(c, __VA_ARGS__))
#define NISERIALIZE_29(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_28(c, __VA_ARGS__))
#define NISERIALIZE_30(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_29(c, __VA_ARGS__))
#define NISERIALIZE_31(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_30(c, __VA_ARGS__))
#define NISERIALIZE_32(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_31(c, __VA_ARGS__))
#define NISERIALIZE_33(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_32(c, __VA_ARGS__))
#define NISERIALIZE_34(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_33(c, __VA_ARGS__))
#define NISERIALIZE_35(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_34(c, __VA_ARGS__))
#define NISERIALIZE_36(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_35(c, __VA_ARGS__))
#define NISERIALIZE_37(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_36(c, __VA_ARGS__))
#define NISERIALIZE_38(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_37(c, __VA_ARGS__))
#define NISERIALIZE_39(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_38(c, __VA_ARGS__))
#define NISERIALIZE_40(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_39(c, __VA_ARGS__))
#define NISERIALIZE_41(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_40(c, __VA_ARGS__))
#define NISERIALIZE_42(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_41(c, __VA_ARGS__))
#define NISERIALIZE_43(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_42(c, __VA_ARGS__))
#define NISERIALIZE_44(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_43(c, __VA_ARGS__))
#define NISERIALIZE_45(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_44(c, __VA_ARGS__))
#define NISERIALIZE_46(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_45(c, __VA_ARGS__))
#define NISERIALIZE_47(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_46(c, __VA_ARGS__))
#define NISERIALIZE_48(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_47(c, __VA_ARGS__))
#define NISERIALIZE_49(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_48(c, __VA_ARGS__))
#define NISERIALIZE_50(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_49(c, __VA_ARGS__))
#define NISERIALIZE_51(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_50(c, __VA_ARGS__))
#define NISERIALIZE_52(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_51(c, __VA_ARGS__))
#define NISERIALIZE_53(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_52(c, __VA_ARGS__))
#define NISERIALIZE_54(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_53(c, __VA_ARGS__))
#define NISERIALIZE_55(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_54(c, __VA_ARGS__))
#define NISERIALIZE_56(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_55(c, __VA_ARGS__))
#define NISERIALIZE_57(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_56(c, __VA_ARGS__))
#define NISERIALIZE_58(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_57(c, __VA_ARGS__))
#define NISERIALIZE_59(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_58(c, __VA_ARGS__))
#define NISERIALIZE_60(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_59(c, __VA_ARGS__))
#define NISERIALIZE_61(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_60(c, __VA_ARGS__))
#define NISERIALIZE_62(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_61(c, __VA_ARGS__))
#define NISERIALIZE_63(c, p, ...) NISERIALIZE_0(c, p) . NI_EXPAND(NISERIALIZE_62(c, __VA_ARGS__))


#define NI_MAKE_CALL(t, f) t . f

#define NI_PARAMS_COUNT(TAG, _63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,\
                        _50,_49,_48,_47,_46,_45,_44,_43,_42,_41,\
                        _40,_39,_38,_37,_36,_35,_34,_33,_32,_31,\
                        _30,_29,_28,_27,_26,_25,_24,_23,_22,_21,\
                        _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,\
                        _10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N

#define NISERIALIZE(t, c, ...) NI_MAKE_CALL(t, NI_EXPAND(NI_PARAMS_COUNT(NISERIALIZE, __VA_ARGS__, \
                        _63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,\
                        _50,_49,_48,_47,_46,_45,_44,_43,_42,_41,\
                        _40,_39,_38,_37,_36,_35,_34,_33,_32,_31,\
                        _30,_29,_28,_27,_26,_25,_24,_23,_22,_21,\
                        _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,\
                        _10,_9,_8,_7,_6,_5,_4,_3,_2,_1))(c, __VA_ARGS__))


#define VISITSTRUCT(structType, ...)                    \
template<typename T>                                    \
FORCEINLINEvoid serialize(T& t, structType& data) {     \
    NISERIALIZE(t, data, __VA_ARGS__);                  \
}


#endif
