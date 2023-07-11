#pragma once

#include "Common/config.h"

#include <cstdint>
#include <climits>

namespace ph
{

/*! @brief Fixed-size integer types.
*/
///@{
using int8    = std::int8_t;
using uint8   = std::uint8_t;
using int16   = std::int16_t;
using uint16  = std::uint16_t;
using int32   = std::int32_t;
using uint32  = std::uint32_t;
using int64   = std::int64_t;
using uint64  = std::uint64_t;
///@}

/*! @brief Fastest integer types with size guarantee.
For example, `uint32f` is an unsigned integer with at least 32 bits.
*/
///@{
using int8f   = std::int_fast8_t;
using uint8f  = std::uint_fast8_t;
using int16f  = std::int_fast16_t;
using uint16f = std::uint_fast16_t;
using int32f  = std::int_fast32_t;
using uint32f = std::uint_fast32_t;
using int64f  = std::int_fast64_t;
using uint64f = std::uint_fast64_t;
///@}

/*! @brief Fixed-size floating-point types.
*/
///@{
using float32 = float;
using float64 = double;
///@}

#if PH_STRICT_FLOATING_POINT_SIZES
	static_assert(sizeof(float32) * CHAR_BIT == 32);
	static_assert(sizeof(float64) * CHAR_BIT == 64);
#endif

#if PH_USE_DOUBLE_REAL
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
