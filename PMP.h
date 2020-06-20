#pragma once
#include "Types.h"

namespace PMP
{
	struct TransfMtx
	{
		f32 firstColumn[3];
		f32 secondColumn[3];
		f32 thirdColumn[3];
	};

	struct Object
	{
		u16 groupID;
		u16 objID;
		u32 unk1;
		f32 posVec[3];
		f32 scaleVec[3];
		TransfMtx Mtx;
		u32 unk2;
		u8 exParams[16];
	};


} // namespace PMP