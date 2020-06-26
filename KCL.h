#pragma once
#include "Types.h"

namespace KCL
{
	struct Triangle
	{
		f32 length;
		u16 posIndex;
		u16 dirIndex;
		u16 normalA;
		u16 normalB;
		u16 normalC;
		u16 flag;
	};
} // namespace KCL