#include <cstring>
#include "RPUtlMapData.h"
#include "core/RPGame/RPGolFieldManager.h"
#include "core/endian.h"

RPUtlMapData::RPUtlMapData(void* pMap)
    : mpHeader(nullptr), mpObjects(nullptr), mpPaths(nullptr), mpPoints(nullptr), mIsBigEndian(true)
{
    assert(pMap);

    // Assign header
    SPmpHeader* header = static_cast<SPmpHeader*>(pMap);
    assert(BIG_ENDIAN_32(header->mMagic) == cPackedMapMagic);
    mpHeader = header;

    // Assign objects
    if (mpHeader->mObjectOfs)
        mpObjects = reinterpret_cast<RPUtlMapObject*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mObjectOfs));

    // Assign paths
    if (mpHeader->mPathOfs)
        mpPaths = reinterpret_cast<RPUtlMapPath*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mPathOfs));

    // Assign points
    if (mpHeader->mPointOfs)
        mpPoints = reinterpret_cast<RPUtlMapPoint*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mPointOfs));
}

void RPUtlMapData::swapEndian()
{
    // The counts only need to be swapped *before* the for loop when the object has not been swapped yet.
    // When saving the file, the object will be in native endian already, so we just use the header's values
    u32 numObjects, numPaths, numPoints;
    numObjects = (mIsBigEndian) ? BIG_ENDIAN_16(mpHeader->mNumObjects) : mpHeader->mNumObjects;
    numPaths = (mIsBigEndian) ? BIG_ENDIAN_16(mpHeader->mNumPaths) : mpHeader->mNumPaths;
    numPoints = (mIsBigEndian) ? BIG_ENDIAN_16(mpHeader->mNumPoints) : mpHeader->mNumPoints;

    u32 i;

    for (i = 0; i < numObjects; i++)
        mpObjects[i].swapEndian();

    for (i = 0; i < numPaths; i++)
        mpPaths[i].swapEndian();

    for (i = 0; i < numPoints; i++)
        mpPoints[i].swapEndian();

    mpHeader->mMagic = BIG_ENDIAN_32(mpHeader->mMagic);
    mpHeader->mNumObjects = BIG_ENDIAN_16(mpHeader->mNumObjects);
    mpHeader->mNumPaths = BIG_ENDIAN_16(mpHeader->mNumPaths);
    mpHeader->mNumPoints = BIG_ENDIAN_16(mpHeader->mNumPoints);
    mpHeader->mObjectOfs = BIG_ENDIAN_32(mpHeader->mObjectOfs);
    mpHeader->mPathOfs = BIG_ENDIAN_32(mpHeader->mPathOfs);
    mpHeader->mPointOfs = BIG_ENDIAN_32(mpHeader->mPointOfs);

    mIsBigEndian = !mIsBigEndian;
}

void RPUtlMapData::deleteObject(u32 i)
{
    assert(mpHeader->mNumObjects > i);

    // Offset into file of start of object i
    u32 objOfs = sizeof(SPmpHeader) + i * sizeof(RPUtlMapObject);
    // Length from object i to end of file
    u32 len = getFileSize() - objOfs;
    // Skip over current object
    len -= sizeof(RPUtlMapObject);

    // Pointer to start of object i
    char* dest = (char*)mpHeader + objOfs;
    // Pointer to end of object i
    char* src = dest + sizeof(RPUtlMapObject);

    // Copy rest of file back one object (overwrite obj i)
    std::memcpy(dest, src, len);
    mpHeader->mNumObjects--;

    // Update offsets
    u32 size = getFileSize();
    mpHeader->mPathOfs = size;
    mpHeader->mPointOfs = size;
}

u32 RPUtlMapData::getFileSize() const
{
    u32 objSize = mpHeader->mNumObjects * sizeof(RPUtlMapObject);
    u32 pathSize = mpHeader->mNumPaths * sizeof(RPUtlMapPath);
    u32 pointSize = mpHeader->mNumPoints * sizeof(RPUtlMapPoint);

    return sizeof(SPmpHeader) + objSize + pathSize + pointSize;
}

void RPUtlMapData::convert()
{
    if (mpHeader->mNumObjects <= 0) return;

    for (u32 i = 0; i < mpHeader->mNumObjects; i++)
    {
        // Remove Frisbee golf objects, and objects which have no OGWS equivalent
        if (mpObjects[i].mGroupID == GRP_WS2_FRISBEE
            || mpObjects[i].mFullObjectID > OBJ_WS2_GLF_TEEOBJ)
        {
            deleteObject(i);
            i--;
        }
        else
        {
            mpObjects[i].convert();
        }
    }
}
