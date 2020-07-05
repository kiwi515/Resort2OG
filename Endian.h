// Byte-swapping functions are modified from LLVM's "SwapByteOrder.h"
// See here: https://llvm.org/doxygen/SwapByteOrder_8h_source.html
#pragma once
#include "Types.h"

enum class ByteOrder
{
	LITTLE_ENDIAN = 0x0,
	BIG_ENDIAN = 0x1
};

// Endianness check
// http://stackoverflow.com/a/8979034/238609
inline int IsBigEndian()
{
	union
	{
		unsigned int i;
		char c[sizeof(unsigned int)];
	} u;
	u.i = 1;
	return !u.c[0];
}

// Convert u16 to specified endianness
inline u16 ToEndian_16(u16 value, ByteOrder order)
{
	if (order == ByteOrder::BIG_ENDIAN)
	{
		if (!IsBigEndian())
		{
			u16 Hi = value << 8;
			u16 Lo = value >> 8;
			return Hi | Lo;
		}
		else return value;
	}
	else
	{
		if (IsBigEndian())
		{
			u16 Hi = value << 8;
			u16 Lo = value >> 8;
			return Hi | Lo;
		}
		else return value;
	}
}

// Convert u32 to specified endianness
inline u32 ToEndian_32(u32 value, ByteOrder order)
{
	if (order == ByteOrder::BIG_ENDIAN)
	{
		if (!IsBigEndian())
		{
			u32 Byte0 = value & 0x000000FF;
			u32 Byte1 = value & 0x0000FF00;
			u32 Byte2 = value & 0x00FF0000;
			u32 Byte3 = value & 0xFF000000;
			return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
		}
		else return value;
	}
	else
	{
		if (IsBigEndian())
		{
			u32 Byte0 = value & 0x000000FF;
			u32 Byte1 = value & 0x0000FF00;
			u32 Byte2 = value & 0x00FF0000;
			u32 Byte3 = value & 0xFF000000;
			return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
		}
		else return value;
	}
}

// U32 Bswap with no endian checks, used so ToEndian_64's body isn't super long
inline u32 ByteSwap_32(u32 value)
{
	u32 Byte0 = value & 0x000000FF;
	u32 Byte1 = value & 0x0000FF00;
	u32 Byte2 = value & 0x00FF0000;
	u32 Byte3 = value & 0xFF000000;
	return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
}

// Convert u64 to specified endianness
inline u64 ToEndian_64(u64 value, ByteOrder order)
{
	if (order == ByteOrder::BIG_ENDIAN)
	{
		if (!IsBigEndian())
		{
			u64 Hi = ByteSwap_32(u32(value));
			u32 Lo = ByteSwap_32(u32(value >> 32));
			return (Hi << 32) | Lo;
		}
		else return value;
	}
	else
	{
		if (IsBigEndian())
		{
			u64 Hi = ByteSwap_32(u32(value));
			u32 Lo = ByteSwap_32(u32(value >> 32));
			return (Hi << 32) | Lo;
		}
		else return value;
	}
}
