#pragma once

/*! @file

@brief Miscellaneous math utilities.

If the standard library has the same/similar math utility defined, prefer the
implementation here since most of them is developed with performance regarding
to rendering in mind.
*/

#include "Math/constant.h"
#include "Math/math_fwd.h"
#include "Math/math_table.h"
#include "Utility/utility.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/compiler.h>
#include <Common/math_basics.h>

#include <cmath>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <utility>
#include <limits>
#include <array>
#include <vector>
#include <cstdint>
#include <climits>
#include <bit>
#include <concepts>

#if PH_COMPILER_IS_MSVC
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_umul128)
#endif

namespace ph::math
{

template<typename T>
inline auto matrix2x2(const T e00, const T e01, const T e10, const T e11)
	-> std::array<std::array<T, 2>, 2>
{
	return
	{{
		{{e00, e01}},
		{{e10, e11}}
	}};
}

template<typename T>
inline T squared(const T value)
{
	return value * value;
}

// A fast, without sqrt(), nearly branchless method. Notice that Photon uses y-axis as the up/normal vector. 
// This static method implements the y-is-normal version which is different from the original paper.
// (Reference: Frisvad, Jeppe Revall, "Building an Orthonormal Basis from a 3D Unit Vector Without Normalization", 
// Journal of Graphics Tools, 2012)
// Note: This method seems to have larger numerical error (at least 10^3 larger than my naive method), 
// thus it is not used currently.
//
void form_orthonormal_basis_frisvad(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

/*! @brief Clamps a integer value in [lowerBound, upperBound].
*/
template<typename T, std::enable_if_t<!std::is_floating_point_v<T>, int> = 0>
inline T clamp(const T value, const T lowerBound, const T upperBound)
{
	return std::min(upperBound, std::max(value, lowerBound));
}

/*! @brief Clamps a float value in [lowerBound, upperBound].
If a floating-point value is NaN, its value is clamped to lower bound. Neither lower bound or upper
bound can be NaN, or the method's behavior is undefined.
*/
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline T clamp(const T value, const T lowerBound, const T upperBound)
{
	// Use `fmin` & `fmax` as they have the properties we want (see this function's doc)
	return std::fmin(upperBound, std::fmax(value, lowerBound));
}

/*! @brief Convert radians to degrees.
*/
template<typename T>
inline T to_degrees(const T radians)
{
	return radians * (constant::rcp_pi<T> * T(180));
}

/*! @brief Convert degrees to radians.
*/
template<typename T>
inline T to_radians(const T degrees)
{
	return degrees * (constant::pi<T> / T(180));
}

/*! @brief Extract the sign of @p value.

Function is defined only if the provided value is comparable to 0. Note that the
input is passed by value.

@return 1 when @p value > 0; -1 when @p value < 0; 0 when @p value == 0.
@note This function is branchless.
*/
template<typename T>
inline int sign(const T value)
{
	return (static_cast<T>(0) < value) - (static_cast<T>(0) > value);
}

/*! @brief Gets the minimum power of 2 value that is >= @p value.

Note that if @p is 0, then 0 will be returned.

Reference:
Stanford CG Lab's webpage: "Bit Twiddling Hacks" by Sean Eron Anderson
*/
// TODO: templatize
inline uint32 next_power_of_2(uint32 value)
{
	PH_ASSERT(value <= (1UL << 31));

	--value;

	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;

	return value + 1;
}

/*! @brief Calculate a positive number's base 2 logarithm (floored).

@param value Should be > 0, or the behavior of this method is undefined.
*/
template<typename T>
inline T log2_floor(const T value)
{
	if constexpr(std::is_floating_point_v<T>)
	{
		PH_ASSERT_GT(value, T(0));

		return static_cast<T>(std::floor(std::log2(value)));
	}
	else
	{
		constexpr T NUM_BITS = sizeof(T) * CHAR_BIT;

		static_assert(std::is_integral_v<T>);

		PH_ASSERT_GT(value, T(0));

#if PH_COMPILER_IS_MSVC

		unsigned long first1BitFromLeftIndex = std::numeric_limits<unsigned long>::max();
		if constexpr(sizeof(T) <= sizeof(unsigned long))
		{
			const auto isIndexSet = _BitScanReverse(
				&first1BitFromLeftIndex, 
				static_cast<unsigned long>(value));
			PH_ASSERT_GT(isIndexSet, 0);
		}
		else
		{
			static_assert(sizeof(T) <= sizeof(unsigned __int64),
				"size of T is too large");

			const auto isIndexSet = _BitScanReverse64(
				&first1BitFromLeftIndex, 
				static_cast<unsigned __int64>(value));
			PH_ASSERT_GT(isIndexSet, 0);
		}
		PH_ASSERT_IN_RANGE_INCLUSIVE(first1BitFromLeftIndex, T(0), NUM_BITS - 1);

		return static_cast<T>(first1BitFromLeftIndex);

#elif PH_COMPILER_IS_CLANG || PH_COMPILER_IS_GCC

		if constexpr(sizeof(T) <= sizeof(unsigned int))
		{
			const int numLeftZeros = __builtin_clz(static_cast<unsigned int>(value));
			return static_cast<T>(sizeof(unsigned int) * CHAR_BIT - 1 - numLeftZeros);
		}
		else if constexpr(sizeof(T) <= sizeof(unsigned long))
		{
			const int numLeftZeros = __builtin_clzl(static_cast<unsigned long>(value));
			return static_cast<T>(sizeof(unsigned long) * CHAR_BIT - 1 - numLeftZeros);
		}
		else
		{
			static_assert(sizeof(T) <= sizeof(unsigned long long),
				"size of T is too large");

			const int numLeftZeros = __builtin_clzll(static_cast<unsigned long long>(value));
			return static_cast<T>(sizeof(unsigned long long) * CHAR_BIT - 1 - numLeftZeros);
		}

#endif
	}
}

/*! @brief Retrieve the fractional part of @p value (with the sign unchanged).

The result is not guaranteed to be the same as the bit representation of
<value>'s fractional part. The result is undefined if input value is NaN or
+-Inf.
*/
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline T fractional_part(const T value)
{
	long double integralPart;
	return static_cast<T>(std::modf(static_cast<long double>(value), &integralPart));
}

/*! @brief Wraps an integer around [lower-bound, upper-bound].
	
For example, given a bound [-1, 2], 3 will be wrapped to -1.
*/
template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline T wrap(T value, const T lowerBound, const T upperBound)
{
	PH_ASSERT_GE(upperBound, lowerBound);

	const T rangeSize = upperBound - lowerBound + static_cast<T>(1);

	if(value < lowerBound)
	{
		value += rangeSize * ((lowerBound - value) / rangeSize + static_cast<T>(1));

		// Possibly fail if <value> overflow
		PH_ASSERT_GE(value, lowerBound);
	}

	return lowerBound + (value - lowerBound) % rangeSize;
}

/*! @brief Checks if a number is even.

@param value The number to be tested. Should be an integer type.
*/
template<typename T>
inline bool is_even(const T value)
{
	static_assert(std::is_integral_v<T>,
		"T must be an integer type.");

	return value % 2 == 0;
}

/*! @brief Checks if a number is odd.

@param value The number to be tested. Should be an integer type.
*/
template<typename T>
inline bool is_odd(const T value)
{
	return !is_even(value);
}

// Checks whether the specified vector is within the hemisphere defined by
// the normal vector N. N points to the hemisphere's peak, i.e., theta = 0.
//
bool is_same_hemisphere(const Vector3R& vector, const Vector3R& N);

/*! @brief Sum all values within a container together.
*/
template<typename T>
inline T summation(TSpanView<T> values, const T initialValue = 0)
{
	return std::accumulate(values.begin(), values.end(), initialValue);
}

/*! @brief Multiplies all values within a container together.
*/
template<typename T>
inline T product(TSpanView<T> values, const T initialValue = 1)
{
	T result = initialValue;
	for(auto&& value : values)
	{
		result *= value;
	}
	return result;
}

template<typename NumberType>
inline constexpr NumberType bytes_to_KiB(const std::size_t numBytes)
{
	return static_cast<NumberType>(numBytes) / static_cast<NumberType>(constant::KiB);
}

template<typename NumberType>
inline constexpr NumberType bytes_to_MiB(const std::size_t numBytes)
{
	return static_cast<NumberType>(numBytes) / static_cast<NumberType>(constant::MiB);
}

template<typename NumberType>
inline constexpr NumberType bytes_to_GiB(const std::size_t numBytes)
{
	return static_cast<NumberType>(numBytes) / static_cast<NumberType>(constant::GiB);
}

template<typename NumberType>
inline constexpr NumberType bytes_to_TiB(const std::size_t numBytes)
{
	return static_cast<NumberType>(numBytes) / static_cast<NumberType>(constant::TiB);
}

template<typename NumberType>
inline constexpr NumberType bytes_to_PiB(const std::size_t numBytes)
{
	return static_cast<NumberType>(numBytes) / static_cast<NumberType>(constant::PiB);
}

/*! @brief Gets the i-th evenly divided range.

Gets the i-th range [beginIndex, endIndex) which is the result of dividing
`totalSize` into `numDivisions` parts as evenly as possible.
*/
inline std::pair<std::size_t, std::size_t> ith_evenly_divided_range(
	const std::size_t rangeIndex, 
	const std::size_t totalSize,
	const std::size_t numDivisions)
{
	// TODO: it is possible to generalize to signed range
	// maybe use ith_evenly_divided_size() as function name and 
	// ith_evenly_divided_range() for signed/unsigned range

	PH_ASSERT_GT(numDivisions, 0);
	PH_ASSERT_LT(rangeIndex, numDivisions);

	return
	{
		rangeIndex * totalSize / numDivisions,
		(rangeIndex + 1) * totalSize / numDivisions
	};
}

/*! @brief Computes `1/sqrt(x)` in a fast but approximative way.

This method is best known for its implementation in Quake III Arena (1999).
Here the implementation follows what described in the referenced paper,
which uses a slightly better (in terms of maximal relative error) magic
number. 

Reference: http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
*/
inline float fast_rcp_sqrt(float x)
{
	// TODO: 
	// 1. implement double version in the reference paper and templatize iteration step
	// 2. is more iteration steps better than doing it in double
	// 3. establish a standard, e.g., fast_<X>() is guaranteed to have max. rel. error < 1%

	PH_ASSERT_GT(x, 0.0f);

	const float halvedInput = 0.5f * x;

	// Gives initial guess for later refinements
	auto bits = bitwise_cast<std::uint32_t>(x);
	bits = 0x5F375A86 - (bits >> 1);
	x = bitwise_cast<float>(bits);

	// Newton's method, each iteration increases accuracy.

	// Iteration 1, max. relative error < 0.175125%
	x = x * (1.5f - halvedInput * x * x);

	// Iteration 2, disabled since <x> is already good enough
	//x = x * (1.5f - halvedInput * x * x);

	return x;
}

/*! @brief Computes sqrt(x) in a fast but approximative way.
*/
inline float fast_sqrt(const float x)
{
	return fast_rcp_sqrt(x) * x;
}

/*! @brief Get an integral value with reversed bits.

Reference:
The lookup table method from Stanford CG Lab's webpage: "Bit Twiddling Hacks"
by Sean Eron Anderson.
*/
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
inline T reverse_bits(const T value)
{
	using namespace detail;

	static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>,
		"Unsupported type detected");

	constexpr std::size_t NUM_BITS = sizeof(T) * CHAR_BIT;

	// Note that arbitrary number of bits can be supported by careful handling of bitwise operations
	static_assert(NUM_BITS % 8 == 0,
		"Non-multiple-of-8 integer bits are not supported");

	if constexpr(NUM_BITS == 8)
	{
		return T(table::detail::BITS8_REVERSE[value]);
	}
	else if constexpr(NUM_BITS == 16)
	{
		return (T(table::detail::BITS8_REVERSE[value        & 0xFF]) << 8) |
		       (T(table::detail::BITS8_REVERSE[(value >> 8) & 0xFF]));
	}
	else if constexpr(NUM_BITS == 32)
	{
		return (T(table::detail::BITS8_REVERSE[value         & 0xFF]) << 24) |
		       (T(table::detail::BITS8_REVERSE[(value >> 8)  & 0xFF]) << 16) |
		       (T(table::detail::BITS8_REVERSE[(value >> 16) & 0xFF]) << 8)  |
		       (T(table::detail::BITS8_REVERSE[(value >> 24) & 0xFF]));
	}
	else
	{
		static_assert(NUM_BITS == 64);

		return (T(table::detail::BITS8_REVERSE[value         & 0xFF]) << 56) |
		       (T(table::detail::BITS8_REVERSE[(value >> 8)  & 0xFF]) << 48) |
		       (T(table::detail::BITS8_REVERSE[(value >> 16) & 0xFF]) << 40) |
		       (T(table::detail::BITS8_REVERSE[(value >> 24) & 0xFF]) << 32) |
		       (T(table::detail::BITS8_REVERSE[(value >> 32) & 0xFF]) << 24) |
		       (T(table::detail::BITS8_REVERSE[(value >> 40) & 0xFF]) << 16) |
		       (T(table::detail::BITS8_REVERSE[(value >> 48) & 0xFF]) << 8)  |
		       (T(table::detail::BITS8_REVERSE[(value >> 56) & 0xFF]));
	}
}

/*! @brief Set bits in the range to 1.
The bits in [beginBitIdx, endBitIdx) will be set to 1, while the rest remain the same.
LSB has the bit index 0.
*/
template<std::unsigned_integral UIntType, std::integral RangeType>
inline UIntType set_bits_in_range(const UIntType bits, const RangeType beginBitIdx, const RangeType endBitIdx)
{
	// Inclusive as empty range is allowed, e.g., `beginBitIdx` == `endBitIdx`
	PH_ASSERT_IN_RANGE_INCLUSIVE(beginBitIdx, 0, endBitIdx);
	PH_ASSERT_IN_RANGE_INCLUSIVE(endBitIdx, beginBitIdx, sizeof_in_bits<UIntType>());

	// Mask for the bits in range. Constructed by producing required number of 1's then shift
	// by <beginBitIdx>. 
	// Note that if there is integer promotion to signed types, it should be fine--promoted type
	// must be wider than the original unsigned type, the bit shifts will never reach sign bit. 
	//
	const auto bitMask = static_cast<UIntType>(((UIntType(1) << (endBitIdx - beginBitIdx)) - 1) << beginBitIdx);

	return bits | bitMask;
}

/*! @brief Set bits in the range to 0.
The bits in [beginBitIdx, endBitIdx) will be set to 0, while the rest remain the same.
LSB has the bit index 0.
*/
template<std::unsigned_integral UIntType, std::integral RangeType>
inline UIntType clear_bits_in_range(const UIntType bits, const RangeType beginBitIdx, const RangeType endBitIdx)
{
	const auto bitMask = set_bits_in_range<UIntType, RangeType>(UIntType(0), beginBitIdx, endBitIdx);
	return bits & (~bitMask);
}

/*! @brief Set a single bit to 1, others remain 0.
LSB has the bit index 0.
*/
///@{
template<std::unsigned_integral UIntType>
inline UIntType flag_bit(const UIntType bitIdx)
{
	PH_ASSERT_IN_RANGE(bitIdx, 0, sizeof_in_bits<UIntType>());

	return static_cast<UIntType>(1) << bitIdx;
}

template<std::unsigned_integral UIntType, UIntType BIT_IDX>
inline consteval UIntType flag_bit()
{
	static_assert(0 <= BIT_IDX && BIT_IDX < sizeof_in_bits<UIntType>(),
		"BIT_IDX must be within [0, # bits in UIntType)");

	return static_cast<UIntType>(1) << BIT_IDX;
}
///@}

template<typename T, T MIN, T MAX, std::size_t N>
inline std::array<T, N> evenly_spaced_array()
{
	static_assert(MAX > MIN);
	static_assert(N >= 2);

	constexpr T TOTAL_SIZE = MAX - MIN;

	std::array<T, N> arr;
	arr[0] = MIN;
	for(std::size_t i = 1; i < N - 1; ++i)
	{
		// Calculate current fraction i/(N-1), and multiply it with the total size
		// (we multiply total size with i fist, otherwise integral types will always result in 0)
		arr[i] = MIN + static_cast<T>(i * TOTAL_SIZE / (N - 1));

		PH_ASSERT_IN_RANGE_INCLUSIVE(arr[i], MIN, MAX);
	}
	arr[N - 1] = MAX;

	return arr;
}

template<typename T>
inline std::vector<T> evenly_spaced_vector(const T min, const T max, const std::size_t n)
{
	PH_ASSERT_GT(max, min);
	PH_ASSERT_GE(n, 2);

	const T totalSize = max - min;

	std::vector<T> vec(n, 0);
	vec[0] = min;
	for(std::size_t i = 0; i < n; ++i)
	{
		// Calculate current fraction i/(n-1), and multiply it with the total size
		// (we multiply total size with i fist, otherwise integral types will always result in 0)
		vec[i] = min + static_cast<T>(i * totalSize / (n - 1));

		PH_ASSERT_IN_RANGE_INCLUSIVE(vec[i], min, max);
	}
	vec[n - 1] = max;
	return vec;
}

/*!
The implementation of half floats does not contain "safety checks", i.e., according to IEEE 754-2008, 
+/-65504 is the max representable value of half floats, input values that exceed the max value will
not be represented correctly and will cause problems in OpenGL or other APIs.
If the input (fp32) is too samll, 0 will be returned.
*/
// TODO: handle denormalized value and others (std::frexp seems interesting)
// https://stackoverflow.com/questions/6162651/half-precision-floating-point-in-java
///@{
/*! @brief Convert a 32-bit float to 16-bit representation.
*/
inline uint16 fp32_to_fp16_bits(const float32 value)
{
	static_assert(std::numeric_limits<float32>::is_iec559);

	if(std::abs(value) < 0.0000610352f)
	{
		return std::signbit(value) ? 0x8000 : 0x0000;
	}
		
	uint16 fp16Bits;
	uint32 fp32Bits;
	    
	fp32Bits = std::bit_cast<uint32>(value);
	    
	// truncate the mantissa and doing (exp - 127 + 15)
	fp16Bits =  static_cast<uint16>((((fp32Bits & 0x7FFFFFFF) >> 13) - (0x38000000 >> 13)));
	fp16Bits |= ((fp32Bits & 0x80000000) >> 16);// sign

	return fp16Bits;
}

/*! @brief Convert a 16-bit representation back to 32-bit float.
*/
inline float32 fp16_bits_to_fp32(const uint16 fp16Bits)
{
	static_assert(std::numeric_limits<float32>::is_iec559);

	// 0 if all bits other than the sign bit are 0
	if((fp16Bits & 0x7FFF) == 0)
	{
		return (fp16Bits & 0x8000) == 0 ? 0.0f : -0.0f;
	}

	uint32 fp32Bits = ((fp16Bits & 0x8000) << 16);
	fp32Bits |= (((fp16Bits & 0x7FFF) << 13) + 0x38000000);

	return std::bit_cast<float32>(fp32Bits);
}
///@}

/*! @brief Transform an integer to the range [0, 1] ([-1, 1] for signed integer).
When transforming an unsigned integer, the minimum value the integer can hold will be mapped to 0, while 
the maximum value will be mapped to 1; the rest of the integer values will be uniformally mapped to the 
range [0, 1]. Signed integer types follow the same rule, except the mapped range will be [-1, 1].
*/
// TODO: numeric analysis
// TODO: long double is not needed for shorter integer types
template<std::floating_point FloatType, std::integral IntType>
inline FloatType normalize_integer(const IntType intVal)
{
	if constexpr(std::is_unsigned_v<IntType>)
	{
		return static_cast<FloatType>(
			static_cast<long double>(intVal) / std::numeric_limits<IntType>::max());
	}
	else
	{
		return static_cast<FloatType>(intVal >= 0
			?  static_cast<long double>(intVal) / std::numeric_limits<IntType>::max()
			: -static_cast<long double>(intVal) / std::numeric_limits<IntType>::min());
	}
}

// TODO: numeric analysis
// TODO: long double is not needed for shorter integer types
template<std::integral IntType, std::floating_point FloatType>
inline IntType quantize_normalized_float(const FloatType floatVal)
{
	if constexpr(std::is_unsigned_v<IntType>)
	{
		PH_ASSERT_IN_RANGE_INCLUSIVE(floatVal, 0.0f, 1.0f);

		return static_cast<IntType>(
			std::round(static_cast<long double>(floatVal) * std::numeric_limits<IntType>::max()));
	}
	else
	{
		PH_ASSERT_IN_RANGE_INCLUSIVE(floatVal, -1.0f, 1.0f);

		return static_cast<IntType>(floatVal >= 0
			? std::round( static_cast<long double>(floatVal) * std::numeric_limits<IntType>::max())
			: std::round(-static_cast<long double>(floatVal) * std::numeric_limits<IntType>::min()));
	}
}

/*! @brief Multiply two unsigined 64-bit numbers and get a 128-bit result.
@param[out] out_high64 The higher 64 bits of the 128-bit result.
@param[out] out_low64 The lower 64 bits of the 128-bit result.
*/
inline void uint64_mul(const uint64 lhs, const uint64 rhs, uint64& out_high64, uint64& out_low64)
{
#if defined(__SIZEOF_INT128__)
	const auto result128 = __uint128_t(lhs) + __uint128_t(rhs);
	out_high64 = static_cast<uint64>(result128 >> 64);
	out_low64 = static_cast<uint64>(result128);
#elif PH_COMPILER_IS_MSVC
	out_low64 = _umul128(lhs, rhs, &out_high64);
#else
	/*
	Divide each 64-bit number into two 32-bit parts, then multiplying (or adding) any 32-bit
	pairs cannot overflow if done using 64-bit arithmetic. If we multiply them using base 2^32 
	arithmetic, a total of 4 products must be shifted and added to obtain the final result. 
	Here is a simple illustration of the algorithm:

	lhs = [ a ][ b ]
	rhs = [ c ][ d ]

	Treat this like performing a regular multiplication on paper. Note that the multiplication
	is done in base 2^32:

	           [ a ][ b ]
	  x        [ c ][ d ]
	  -------------------
	           [a*d][b*d]
	  +   [a*c][b*c]
	  -------------------

	  The overflowing part of each partial product is the carry (32-bit carry). This is a good reference:
	  https://stackoverflow.com/questions/26852435/reasonably-portable-way-to-get-top-64-bits-from-64x64-bit-multiply
	*/

	const uint64 a = lhs >> 32, b = lhs & 0xFFFFFFFF;
	const uint64 c = rhs >> 32, d = rhs & 0xFFFFFFFF;

	const uint64 ac = a * c;
	const uint64 bc = b * c;
	const uint64 ad = a * d;
	const uint64 bd = b * d;

	// This is the middle part of the above illustration, which is a sum of three 32-bit numbers 
	// (so it is 34-bit at most): two products b*c and a*d, and the carry from b*d
	const uint64 mid34 = (bd >> 32) + (bc & 0xFFFFFFFF) + (ad & 0xFFFFFFFF);

	out_high64 = ac + (bc >> 32) + (ad >> 32) + (mid34 >> 32);
	out_low64 = (mid34 << 32) | (bd & 0xFFFFFFFF);
#endif
}

}// end namespace ph::math
