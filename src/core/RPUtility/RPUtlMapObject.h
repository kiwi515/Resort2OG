#pragma once
#include "types.h"
#include "core/egg/eggVector.h"
#include "core/egg/eggMatrix.h"
#include "core/RPGame/RPGolFieldManager.h"

struct RPUtlMapObject
{
    void swapEndian();
    void convert();

    union
    {
        /* 0x00 */ u16 mGroupID;
        /* 0x02 */ u16 mObjectID;

        /* 0x00 */ u32 mFullObjectID;
    };
    /* 0x04 */ u32 UNK_0x8;
    /* 0x08 */ EGG::Vector3f mPos;
    /* 0x14 */ EGG::Vector3f mScale;
    /* 0x20 */ EGG::Matrix33f mTransform;
    /* 0x44 */ u32 UNK_0x44;
    /* 0x48 */ u8 mParams[16];
};
