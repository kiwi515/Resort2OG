#pragma once
#include "types.h"
#include "RPUtlMapObject.h"
#include "RPUtlMapPath.h"
#include "RPUtlMapPoint.h"

struct RPUtlMapData
{
    static constexpr u32 cPackedMapMagic = 'PMPF';

    struct SPmpHeader
    {
        /* 0x00 */ u32 mMagic;
        /* 0x04 */ u8 reserved0[12];
        /* 0x10 */ u16 mNumObjects;
        /* 0x12 */ u16 mNumPaths;
        /* 0x14 */ u16 mNumPoints;
        /* 0x16 */ u8 reserved1[42];
        /* 0x40 */ u32 mObjectOfs;
        /* 0x44 */ u32 mPathOfs;
        /* 0x48 */ u32 mPointOfs;
        /* 0x4C */ u8 reserved2[52];
    };

    RPUtlMapData(void* pMap);
    ~RPUtlMapData() = default;

    void deleteObject(u32);
    void swapEndian();
    u32 getFileSize() const;
    void convert();

    /* 0x00 */ SPmpHeader* mpHeader;
    /* 0x04 */ RPUtlMapObject* mpObjects;
    /* 0x08 */ RPUtlMapPath* mpPaths;
    /* 0x0C */ RPUtlMapPoint* mpPoints;
    /* 0x10 */ bool mIsBigEndian;
};
