#pragma once
#include "types.h"

struct RPUtlMapPath
{
    static constexpr u32 cPathNameLen = 18;

    void swapEndian();

    /* 0x00 */ u16 mNumPoints;
    /* 0x02 */ u32 UNK_0x2;
    /* 0x06 */ u16 mGroupID;
    /* 0x08 */ char mName[cPathNameLen];
    /* 0x1A */ u16 mFirstPointIdx;
    /* 0x1C */ u32 UNK_0x1C;
};
