#include "RPUtlMapPoint.h"
#include "types.h"
#include "core/endian.h"

using namespace EGG;

void RPUtlMapPoint::swapEndian()
{
    for (int i = 0; i < (sizeof(Vector3f) / sizeof(f32)); i++)
        mPos[i] = BIG_ENDIAN_F32(mPos[i]);
}
