#pragma once
#include "types.h"

inline bool usingBigEndian()
{
    union
    {
        u16 i;
        char j[sizeof(u16)];
    };

    i = 1;
    return j[sizeof(u16) - 1] == 1;
}

inline u16 swap16(u16 i)
{
    return (i << 8 | i >> 8);
}

inline u32 swap32(u32 i)
{
    return ((swap16(i & 0xFFFF) << 16) | swap16(i >> 16));
}

inline f32 swapSingle(f32 f)
{
    union
    {
        f32 i;
        u32 j;
    };

    i = f;
    j = swap32(j);
    return i;
}

#define BIG_ENDIAN_16(x) \
((usingBigEndian()) ? x : swap16(x))

#define BIG_ENDIAN_32(x) \
((usingBigEndian()) ? x : swap32(x))

#define BIG_ENDIAN_F32(x) \
((usingBigEndian()) ? x : swapSingle(x))
