#pragma once

#include "Common/config.h"

#include <cstdint>
#include <climits>

namespace ph
{

using int8    = std::int8_t;
using uint8   = std::uint8_t;
using int16   = std::int16_t;
using uint16  = std::uint16_t;
using int32   = std::int32_t;
using uint32  = std::uint32_t;
using int64   = std::int64_t;
using uint64  = std::uint64_t;

using float32 = float;
using float64 = double;

#ifdef PH_STRICT_FLOATING_POINT_SIZES
	static_assert(sizeof(float32) * CHAR_BIT == 32);
	static_assert(sizeof(float64) * CHAR_BIT == 64);
#endif

#ifdef PH_USE_DOUBLE_REAL
	using real = double;
#else
	using real = float;
#endif

using integer   = int;
using hiReal    = float64;
using hiInteger = int64;

constexpr inline real operator "" _r(const long double cookedValue)
{
	return static_cast<real>(cookedValue);
}

}// end namespace ph
