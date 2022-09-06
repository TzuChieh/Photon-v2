#pragma once

#include "Common/assertion.h"

#include <type_traits>
#include <bit>
#include <concepts>
#include <limits>

namespace ph::math
{

/*! @brief Determines whether @p value is a power of 2 number.
*/
template<typename T>
inline constexpr bool is_power_of_2(const T value)
{
	if constexpr(std::is_unsigned_v<T>)
	{
		// STL has this function for unsigned types only
		return std::has_single_bit(value);
	}
	else
	{
		return (value > 0) && !(value & (value - 1));
	}
}

/*! @brief Divide @p numerator by @p denominator and round up to integer.
Both inputs must be positive integer. Specifically, `numerator` >= 0 and `denominator` > 0.
*/
template<std::integral T>
inline T ceil_div(const T numerator, const T denominator)
{
	PH_ASSERT_GE(numerator, 0);
	PH_ASSERT_GT(denominator, 0);

	// Check for possible overflow when doing `numerator` + (`denominator` - 1).
	// (i.e., MAX >= `numerator` + (`denominator` - 1), rearranged to minimize overflow)
	PH_ASSERT_GE(std::numeric_limits<T>::max() - numerator, denominator - 1);

	// We group the (`denominator` - 1) together as `numerator` is usually far larger than
	// `denominator`, doing (`denominator` - 1) first has better chance to avoid an overflow
	return (numerator + (denominator - 1)) / denominator;
}

/*! @brief Get the next number that is an integer multiple of @p multiple.
Specifically, get the minimum number x = C * `multiple` >= `value` where C is an integer >= 0.
Currently supports positive integers only.
*/
template<std::integral T>
inline T next_multiple(const T value, const T multiple)
{
	PH_ASSERT_GT(multiple, 0);

	return ceil_div(value, multiple) * multiple;
}

/*! @brief Same as next_multiple(T, T) except that @p multiple must be a power of 2 number.
*/
template<std::integral T>
inline T next_power_of_2_multiple(const T value, const T multiple)
{
	PH_ASSERT_GE(value, 0);
	PH_ASSERT(is_power_of_2(multiple));
	
	// Reference: https://stackoverflow.com/a/9194117

	// Check for possible overflow when doing `value` + (`multiple` - 1).
	// (see the implementation of `ceil_div()` for details about this test)
	PH_ASSERT_GE(std::numeric_limits<T>::max() - value, multiple - 1);

	// `& ~(multiple - 1)` is the same as `& -multiple` for two's complement arithmetic
	return (value + (multiple - 1)) & (0 - multiple);

	// Note:
	// MSVC may emit C4146 warning on applying a unary negate operation on unsigned types. Subtracting a
	// value from zero is the same as taking its negative, but using the binary subtraction operator avoids
	// the warning about taking the negative of an unsigned value. Hence the `0 - multiple` instead of `-multiple`.
	// Reference: https://stackoverflow.com/a/26893482
}

}// end namespace ph::math
