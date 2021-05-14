#include "RPUtlMapPath.h"
#include "types.h"
#include "core/endian.h"

void RPUtlMapPath::swapEndian()
{
    mNumPoints = BIG_ENDIAN_16(mNumPoints);
    UNK_0x2 = BIG_ENDIAN_32(UNK_0x2);
    mGroupID = BIG_ENDIAN_16(mGroupID);
    mFirstPointIdx = BIG_ENDIAN_16(mFirstPointIdx);
    UNK_0x1C = BIG_ENDIAN_32(UNK_0x1C);
}