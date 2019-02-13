#pragma once

#include "Common/config.h"

#include <cstdint>

namespace ph
{

using int8    = int8_t;
using uint8   = uint8_t;
using int16   = int16_t;
using uint16  = uint16_t;
using int32   = int32_t;
using uint32  = uint32_t;
using int64   = int64_t;
using uint64  = uint64_t;
using float32 = float;
using float64 = double;

#ifdef PH_USE_DOUBLE_REAL
	typedef double real;
#else
	typedef float real;
#endif

using integer   = int;
using hiReal    = float64;
using hiInteger = int64;

constexpr inline real operator "" _r(const long double cookedValue)
{
	return static_cast<real>(cookedValue);
}

}// end namespace ph
