#pragma once
#include "types.h"
#include "core/egg/eggVector.h"

struct RPUtlMapPoint
{
    void swapEndian();

    /* 0x00 */ EGG::Vector3f mPos;
    /* 0x0C */ u8 mParams[8];
};
