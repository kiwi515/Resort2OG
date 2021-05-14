#pragma once
#include "types.h"
#include "eggVector.h"
#include <cassert>

namespace EGG
{
    struct Matrix33f
    {
        inline Vector3f& operator[](int i)
        {
            assert(i < sizeof(mData) / sizeof(mData[0]));
            return mData[i];
        }

        Vector3f mData[3];
    };
}
