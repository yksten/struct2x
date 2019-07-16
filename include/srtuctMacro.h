#ifndef __JSON_MACRO_H__
#define __JSON_MACRO_H__


#define SERIALIZATION_1(t, p1) t.convert(#p1, p1)
#define SERIALIZATION_2(t, p1, p2) SERIALIZATION_1(t, p1).convert(#p2, p2)
#define SERIALIZATION_3(t, p1, p2, p3) SERIALIZATION_2(t, p1, p2).convert(#p3, p3)
#define SERIALIZATION_4(t, p1, p2, p3, p4) SERIALIZATION_3(t, p1, p2, p3).convert(#p4, p4)
#define SERIALIZATION_5(t, p1, p2, p3, p4, p5) SERIALIZATION_4(t, p1, p2, p3, p4).convert(#p5, p5)
#define SERIALIZATION_6(t, p1, p2, p3, p4, p5, p6) SERIALIZATION_5(t, p1, p2, p3, p4, p5).convert(#p6, p6)
#define SERIALIZATION_7(t, p1, p2, p3, p4, p5, p6, p7) SERIALIZATION_6(t, p1, p2, p3, p4, p5, p6).convert(#p7, p7)
#define SERIALIZATION_8(t, p1, p2, p3, p4, p5, p6, p7, p8) SERIALIZATION_7(t, p1, p2, p3, p4, p5, p6, p7).convert(#p8, p8)
#define SERIALIZATION_9(t, p1, p2, p3, p4, p5, p6, p7, p8, p9) SERIALIZATION_8(t, p1, p2, p3, p4, p5, p6, p7, p8).convert(#p9, p9)
#define SERIALIZATION_10(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) SERIALIZATION_9(t, p1, p2, p3, p4, p5, p6, p7, p8, p9).convert(#p10, p10)
#define SERIALIZATION_11(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) SERIALIZATION_10(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10).convert(#p11, p11)
#define SERIALIZATION_12(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) SERIALIZATION_11(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11).convert(#p12, p12)
#define SERIALIZATION_13(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) SERIALIZATION_12(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12).convert(#p13, p13)
#define SERIALIZATION_14(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) SERIALIZATION_13(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13).convert(#p14, p14)
#define SERIALIZATION_15(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) SERIALIZATION_14(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14).convert(#p15, p15)
#define SERIALIZATION_16(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) SERIALIZATION_15(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15).convert(#p16, p16)
#define SERIALIZATION_17(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) SERIALIZATION_16(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16).convert(#p17, p17)
#define SERIALIZATION_18(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) SERIALIZATION_17(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17).convert(#p18, p18)
#define SERIALIZATION_19(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) SERIALIZATION_18(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18).convert(#p19, p19)
#define SERIALIZATION_20(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) SERIALIZATION_19(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19).convert(#p20, p20)

#define X_COUNT(TAG, _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N
#define X_EXPAND(...) __VA_ARGS__
#ifndef _MSC_VER
#define SERIALIZATION(t, ...)  X_COUNT(SERIALIZATION, __VA_ARGS__, _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1) (t, __VA_ARGS__)
#else
#define SERIALIZATION(t, ...)  X_EXPAND(X_COUNT(SERIALIZATION, __VA_ARGS__, _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)) X_EXPAND((t, __VA_ARGS__))
#endif


#endif