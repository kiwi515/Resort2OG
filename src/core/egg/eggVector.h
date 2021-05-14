#pragma once
#include "types.h"
#include <cassert>

namespace EGG
{
    struct Vector3f
    {
        inline f32& operator[](int i)
        {
            assert(i < sizeof(mData) / sizeof(mData[0]));
            return mData[i];
        }

        union
        {
            f32 mData[3];
            f32 x, y, z;
        };
    };
}
