#ifndef __STRUCT_NOINVASIVEMACRO_H__
#define __STRUCT_NOINVASIVEMACRO_H__


#define NISERIALIZATION_1(t, c, p1) t.convert(#p1, c.p1)
#define NISERIALIZATION_2(t, c, p1, p2) NISERIALIZATION_1(t, c, p1).convert(#p2, c.p2)
#define NISERIALIZATION_3(t, c, p1, p2, p3) NISERIALIZATION_2(t, c, p1, p2).convert(#p3, c.p3)
#define NISERIALIZATION_4(t, c, p1, p2, p3, p4) NISERIALIZATION_3(t, c, p1, p2, p3).convert(#p4, c.p4)
#define NISERIALIZATION_5(t, c, p1, p2, p3, p4, p5) NISERIALIZATION_4(t, c, p1, p2, p3, p4).convert(#p5, c.p5)
#define NISERIALIZATION_6(t, c, p1, p2, p3, p4, p5, p6) NISERIALIZATION_5(t, c, p1, p2, p3, p4, p5).convert(#p6, c.p6)
#define NISERIALIZATION_7(t, c, p1, p2, p3, p4, p5, p6, p7) NISERIALIZATION_6(t, c, p1, p2, p3, p4, p5, p6).convert(#p7, c.p7)
#define NISERIALIZATION_8(t, c, p1, p2, p3, p4, p5, p6, p7, p8) NISERIALIZATION_7(t, c, p1, p2, p3, p4, p5, p6, p7).convert(#p8, c.p8)
#define NISERIALIZATION_9(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9) NISERIALIZATION_8(t, c, p1, p2, p3, p4, p5, p6, p7, p8).convert(#p9, c.p9)
#define NISERIALIZATION_10(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) NISERIALIZATION_9(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9).convert(#p10, c.p10)
#define NISERIALIZATION_11(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) NISERIALIZATION_10(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10).convert(#p11, c.p11)
#define NISERIALIZATION_12(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) NISERIALIZATION_11(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11).convert(#p12, c.p12)
#define NISERIALIZATION_13(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) NISERIALIZATION_12(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12).convert(#p13, c.p13)
#define NISERIALIZATION_14(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) NISERIALIZATION_13(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13).convert(#p14, c.p14)
#define NISERIALIZATION_15(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) NISERIALIZATION_14(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14).convert(#p15, c.p15)
#define NISERIALIZATION_16(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) NISERIALIZATION_15(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15).convert(#p16, c.p16)
#define NISERIALIZATION_17(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17) NISERIALIZATION_16(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16).convert(#p17, c.p17)
#define NISERIALIZATION_18(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18) NISERIALIZATION_17(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17).convert(#p18, c.p18)
#define NISERIALIZATION_19(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19) NISERIALIZATION_18(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18).convert(#p19, c.p19)
#define NISERIALIZATION_20(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20) NISERIALIZATION_19(t, c, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19).convert(#p20, c.p20)

#define X_NI_COUNT(TAG, _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N
#define X_NI_EXPAND(...) __VA_ARGS__
#ifndef _MSC_VER
#define NISERIALIZATION(t, c, ...)  X_NI_COUNT(NISERIALIZATION, __VA_ARGS__, _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1) (t, c, __VA_ARGS__)
#else
#define NISERIALIZATION(t, c, ...)  X_NI_EXPAND(X_NI_COUNT(NISERIALIZATION, __VA_ARGS__, _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)) X_NI_EXPAND((t, c, __VA_ARGS__))
#endif


#endif