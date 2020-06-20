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

	struct Route
	{
		u16 routePntCount;
		u32 unk1;
		u16 groupID;
		char routeName[18];
		u16 firstPntIndex;
		u32 unk2;
	};

	struct Point
	{
		f32 posVec[3];
		u8 exParams[12];
	};

	struct Header
	{
		// char magic[4];
		u16 objCount;
		u16 routeCount;
		u16 pntCount;
		u32 objDataOffset;
		u32 routeDataOffset;
		u32 pointDataOffset;
	};

} // namespace PMP