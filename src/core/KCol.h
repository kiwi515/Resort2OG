#pragma once
#include "types.h"
#include "core/egg/eggVector.h"

struct KCol
{
    struct KColHeader
    {
        /* 0x00 */ u32 mVerticesOfs;
        /* 0x04 */ u32 mNormalsOfs;
        /* 0x08 */ u32 mTrianglesOfs;
        /* 0x0C */ u32 mOctreeOfs;
        /* 0x10 */ f32 mThickness;
        /* 0x14 */ EGG::Vector3f mOctreeFirst;
        /* 0x20 */ u32 mMaskX;
        /* 0x24 */ u32 mMaskY;
        /* 0x28 */ u32 mMaskZ;
        /* 0x2C */ u32 mShiftX;
        /* 0x30 */ u32 mShiftY;
        /* 0x34 */ u32 mShiftZ;
        /* 0x38 */ f32 mMaxHitboxRadius;
    };

    struct KColTriangle
    {
        /* 0x00 */ f32 mLength;
        /* 0x04 */ u16 mPosIdx;
        /* 0x06 */ u16 mDirIdx;
        /* 0x08 */ u16 mNrmAIdx;
        /* 0x0A */ u16 mNrmBIdx;
        /* 0x0C */ u16 mNrmCIdx;
        /* 0x0E */ u16 mFlags;
    };

    KCol(void*);
    void swapEndian();
    void convert();

    /* 0x00 */ KColHeader* mpHeader;
    /* 0x04 */ EGG::Vector3f* mpVertices;
    /* 0x08 */ EGG::Vector3f* mpNormals;
    /* 0x0C */ KColTriangle* mpTriangles;
    /* 0x10 */ void* mpOctree;
};
