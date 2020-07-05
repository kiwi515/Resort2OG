#pragma once
#include <cstdint>

typedef signed char         s8;
typedef int16_t             s16;
typedef int32_t             s32;
typedef int64_t             s64;
typedef unsigned char       u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;
typedef float               f32;
typedef double              f64;

enum class Error
{
	INVALID_INPUT = 0x1,
	OPEN_FILE_ERROR = 0x2,
	PMP_INVALID_MAGIC = 0x3,
	PMP_ALREADY_CONVERTED = 0x4
};

enum class FType
{
	EMPTY = 0x0,
	KCL = 0x1,
	PMP = 0x2
};