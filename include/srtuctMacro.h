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

#define X_COUNT(TAG, _10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N
#define X_EXPAND(...) __VA_ARGS__
#ifndef _MSC_VER
#define SERIALIZATION(t, ...)  X_COUNT(SERIALIZATION, __VA_ARGS__, _10,_9,_8,_7,_6,_5,_4,_3,_2,_1) (t, __VA_ARGS__)
#else
#define SERIALIZATION(t, ...)  X_EXPAND(X_COUNT(SERIALIZATION, __VA_ARGS__,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)) X_EXPAND((t, __VA_ARGS__))
#endif


#endif