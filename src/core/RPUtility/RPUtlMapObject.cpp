#include "RPUtlMapObject.h"
#include "core/RPGame/RPGolFieldManager.h"
#include "core/endian.h"

using namespace EGG;

void RPUtlMapObject::swapEndian()
{
    mFullObjectID = BIG_ENDIAN_32(mFullObjectID);
    UNK_0x8 = BIG_ENDIAN_32(UNK_0x8);

    int i, j;
    for (i = 0; i < (sizeof(Vector3f) / sizeof(f32)); i++)
    {
        mPos[i] = BIG_ENDIAN_F32(mPos[i]);
        mScale[i] = BIG_ENDIAN_F32(mScale[i]);
    }

    for (i = 0; i < (sizeof(Matrix33f) / sizeof(Vector3f)); i++)
        for (j = 0; j < (sizeof(Vector3f) / sizeof(f32)); j++)
            mTransform[i][j] = BIG_ENDIAN_F32(mTransform[i][j]);

    UNK_0x44 = BIG_ENDIAN_32(UNK_0x44);
}

void RPUtlMapObject::convert()
{
    // Tree3 conversions alternate between Tree1 and Tree2
    u32 numTrees = 0;

    // Convert full object ID (group ID and object ID)
    switch (mFullObjectID)
    {
    case OBJ_WS2_GLF_PLAYER:
        mFullObjectID = OBJ_OGWS_GOL_PLAYER;
        break;
    case OBJ_WS2_GLF_HOLE:
        mFullObjectID = OBJ_OGWS_GOL_HOLE;
        break;
    case OBJ_WS2_GLF_TREE1:
        mFullObjectID = OBJ_OGWS_GOL_TREE1;
        break;
    case OBJ_WS2_GLF_TREE2:
        mFullObjectID = OBJ_OGWS_GOL_TREE2;
        break;
    case OBJ_WS2_GLF_TREE1ENV:
        mFullObjectID = OBJ_OGWS_GOL_TREE1ENV;
        break;
    case OBJ_WS2_GLF_TREE2ENV:
        mFullObjectID = OBJ_OGWS_GOL_TREE2ENV;
        break;
    case OBJ_WS2_GLF_TREE3:
        mFullObjectID = (numTrees++ % 2 == 0) ? OBJ_OGWS_GOL_TREE1 : OBJ_OGWS_GOL_TREE2;
        break;
    case OBJ_WS2_GLF_TREE3ENV:
        mFullObjectID = (numTrees++ % 2 == 0) ? OBJ_OGWS_GOL_TREE1ENV : OBJ_OGWS_GOL_TREE2ENV;
        break;
    case OBJ_WS2_GLF_SKY:
        mFullObjectID = OBJ_OGWS_GOL_SKY;
        break;
    case OBJ_WS2_GLF_TEEOBJ:
        mFullObjectID = OBJ_OGWS_GOL_TEEOBJ;
        break;
    default:
        assert(false);
        mFullObjectID = OBJ_OGWS_INVALID;
        break;
    }
}
