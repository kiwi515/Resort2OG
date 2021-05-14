#include "KCol.h"
#include "types.h"
#include "core/endian.h"
#include "core/RPGame/RPGolFieldManager.h"

using namespace EGG;

KCol::KCol(void* pKcl)
    : mpHeader(nullptr), mpVertices(nullptr), mpNormals(nullptr), mpTriangles(nullptr), mpOctree(nullptr)
{
    assert(pKcl);

    // Header
    KColHeader* header = static_cast<KColHeader*>(pKcl);
    mpHeader = header;

    // Calculate section ptrs using offsets in header
    if (mpHeader->mVerticesOfs)
        mpVertices = reinterpret_cast<EGG::Vector3f*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mVerticesOfs));

    if (mpHeader->mNormalsOfs)
        mpNormals = reinterpret_cast<EGG::Vector3f*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mVerticesOfs));

    // Triangle section is one indexed
    if (mpHeader->mTrianglesOfs)
        mpTriangles = reinterpret_cast<KColTriangle*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mTrianglesOfs) + sizeof(KColTriangle));

    if (mpHeader->mOctreeOfs)
        mpOctree = reinterpret_cast<void*>((char*)mpHeader + BIG_ENDIAN_32(mpHeader->mOctreeOfs));
}

void KCol::swapEndian()
{
    mpHeader->mVerticesOfs = BIG_ENDIAN_32(mpHeader->mVerticesOfs);
    mpHeader->mNormalsOfs = BIG_ENDIAN_32(mpHeader->mNormalsOfs);
    mpHeader->mTrianglesOfs = BIG_ENDIAN_32(mpHeader->mTrianglesOfs);
    mpHeader->mOctreeOfs = BIG_ENDIAN_32(mpHeader->mOctreeOfs);
}

void KCol::convert()
{
    u32 triangleCount = (mpHeader->mOctreeOfs - mpHeader->mTrianglesOfs) / sizeof(KColTriangle);

    for (u32 i = 0; i < triangleCount; i++)
    {
        switch (BIG_ENDIAN_16(mpTriangles[i].mFlags))
        {
        case KCL_WS2_FAIRWAY:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_FAIRWAY);
            break;
        case KCL_WS2_ROUGH:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_ROUGH);
            break;
        case KCL_WS2_BUNKER:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_BUNKER);
            break;
        case KCL_WS2_OOB:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_OOB);
            break;
        case KCL_WS2_GREEN:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_GREEN);
            break;
        case KCL_WS2_WATER:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_WATER);
            break;
        case KCL_WS2_FRINGE:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_FRINGE);
            break;
        case KCL_WS2_CLIFF:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_CLIFF);
            break;
        case KCL_WS2_CAMERALOCK:
            mpTriangles[i].mFlags = BIG_ENDIAN_16(KCL_OGWS_CAMERALOCK);
            break;
        default:
            break;
        }
    }
}
