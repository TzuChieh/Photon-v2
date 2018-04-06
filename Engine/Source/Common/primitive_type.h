#pragma once

#include "Common/config.h"

#include <cstdint>

namespace ph
{

typedef int8_t         int8;
typedef uint8_t        uint8;
typedef int32_t        int32;
typedef uint32_t       uint32;
typedef int64_t        int64;
typedef uint64_t       uint64;
typedef float          float32;
typedef double         float64;

#ifdef PH_USE_DOUBLE_REAL
	typedef double real;
#else
	typedef float real;
#endif

typedef int     integer;
typedef float64 hiReal;
typedef int64   hiInteger;

constexpr inline real operator "" _r(const long double cookedValue)
{
	return static_cast<real>(cookedValue);
}

}// end namespace ph
