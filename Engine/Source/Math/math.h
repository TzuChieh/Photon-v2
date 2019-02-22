#pragma once

/*! @file

@brief Miscellaneous math utilities.

If the standard library has the same/similar math utility defined, prefer the
implementation here since most of them is developed with performance regarding
to rendering in mind.
*/

#include "Common/primitive_type.h"
#include "Math/constant.h"
#include "Math/math_fwd.h"
#include "Common/compiler.h"
#include "Common/assertion.h"
#include "Utility/utility.h"

#include <cmath>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <limits>
#include <array>
#include <vector>
#include <cstdint>
#include <climits>

#if defined(PH_COMPILER_IS_MSVC)
	#include <intrin.h>
	#pragma intrinsic(_BitScanReverse)
#endif

namespace ph
{

namespace math
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

void form_orthonormal_basis(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

// A fast, without sqrt(), nearly branchless method. Notice that Photon uses y-axis as the up/normal vector. This
// static method implements the y-is-normal version which is different from the original paper.
// (Reference: Frisvad, Jeppe Revall, "Building an Orthonormal Basis from a 3D Unit Vector Without Normalization", 
// Journal of Graphics Tools, 2012)
// Note: This method seems to have larger numerical error (at least 10^3 larger than my naive method), 
// thus it is not used currently.
//
void form_orthonormal_basis_frisvad(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

// Clamp a real value to specific range. If the real value is NaN, its 
// value is clamped to lower bound. Neither lower bound or upper bound 
// can be NaN, or the method's behavior is undefined.

/*! @brief Clamps a integer value in [lowerBound, upperBound].
*/
template<typename T, std::enable_if_t<!std::is_floating_point_v<T>, int> = 0>
inline T clamp(const T value, const T lowerBound, const T upperBound)
{
	return std::min(upperBound, std::max(value, lowerBound));
}

/*! @brief Clamps a float value in [lowerBound, upperBound].
*/
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline T clamp(const T value, const T lowerBound, const T upperBound)
{
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

@return 1 when @p value > 0; -1 when @p value < 0; 0 when @p value == 0

Function is defined only if the provided value is comparable to 0. Note that the
input is passed by value. (current implementation is branchless)
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

/*! @brief Determines whether @p value is a power of 2 number.
*/
template<typename T>
inline bool is_power_of_2(const T value)
{
	return (value > 0) && !(value & (value - 1));
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

#if defined(PH_COMPILER_IS_MSVC)

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

#elif defined(PH_COMPILER_IS_CLANG) || defined(PH_COMPILER_IS_GCC)

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

/*! @brief Solves linear systems of the form Ax = b.

@param A A 2x2 matrix.
@param b A 2x1 vector.
@param[out] out_x A 2x1 vector.
@return If x is successfully solved, method returns `true` and @p out_x stores
the answer; otherwise, `false` is returned and what @p out_x stores is
undefined.
*/
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline bool solve_linear_system_2x2(
	const std::array<std::array<T, 2>, 2>& A,
	const std::array<T, 2>&                b,
	std::array<T, 2>* const                out_x)
{
	PH_ASSERT(!std::numeric_limits<T>::is_integer);

	const T determinant = A[0][0] * A[1][1] - A[1][0] * A[0][1];
	if(std::abs(determinant) <= std::numeric_limits<T>::epsilon())
	{
		return false;
	}

	const T reciDeterminant = 1 / determinant;

	(*out_x)[0] = (A[1][1] * b[0] - A[0][1] * b[1]) * reciDeterminant;
	(*out_x)[1] = (A[0][0] * b[1] - A[1][0] * b[0]) * reciDeterminant;
	return true;
}

/*! @brief Wraps an integer around [lower-bound, upper-bound].
	
For example, given a bound [-1, 2], 3 will be wrapped to -1.
*/
template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline T wrap(T value, const T lowerBound, const T upperBound)
{
	PH_ASSERT(upperBound >= lowerBound);

	const T rangeSize = upperBound - lowerBound + static_cast<T>(1);

	if(value < lowerBound)
	{
		value += rangeSize * ((lowerBound - value) / rangeSize + static_cast<T>(1));
	}

	return lowerBound + (value - lowerBound) % rangeSize;
}

// Checks whether the specified vector is within the hemisphere defined by
// the normal vector N. N points to the hemisphere's peak, i.e., theta = 0.
//
bool is_same_hemisphere(const Vector3R& vector, const Vector3R& N);

/*! @brief Multiplies all values together within a container.
*/
template<typename T>
inline T product(const std::vector<T>& values)
{
	T result(1);
	for(auto&& value : values)
	{
		result *= value;
	}
	return result;
}

template<typename KBType, typename ByteType>
inline KBType byte_to_KB(const ByteType numBytes)
{
	return static_cast<KBType>(numBytes) / KBType(1024);
}

template<typename MBType, typename ByteType>
inline MBType byte_to_MB(const ByteType numBytes)
{
	return static_cast<MBType>(numBytes) / MBType(1024 * 1024);
}

template<typename GBType, typename ByteType>
inline GBType byte_to_GB(const ByteType numBytes)
{
	return static_cast<GBType>(numBytes) / GBType(1024 * 1024 * 1024);
}


/*! @brief Gets the i-th evenly divided range.

Gets the i-th range [beginIndex, endIndex) which is the result of dividing
<totalSize> into <numDivisions> parts as evenly as possible.
*/
// TODO: it is possible to generalize to signed range
//       maybe use ith_evenly_divided_size() as function name and 
//       ith_evenly_divided_range() for signed/unsigned range
inline std::pair<std::size_t, std::size_t> ith_evenly_divided_range(
	const std::size_t rangeIndex, 
	const std::size_t totalSize,
	const std::size_t numDivisions)
{
	PH_ASSERT_GT(numDivisions, 0);
	PH_ASSERT_LT(rangeIndex, numDivisions);

	return {rangeIndex * totalSize / numDivisions,
	        (rangeIndex + 1) * totalSize / numDivisions};
}

/*! @brief Computes 1/sqrt(x) in a fast but approximative way.

	This method is best known for its implementation in Quake III Arena (1999).
	Here the implementation follows what described in the referenced paper,
	which uses a slightly better (in terms of maximal relative error) magic
	number. 

	Reference: http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
*/
// TODO: 
// 1. implement double version in the reference paper and templatize iteration step
// 2. is more iteration steps better than doing it in double
// 3. establish a standard, e.g., fast_<X>() is guaranteed to have max. rel. error < 1%
inline float fast_rcp_sqrt(float x)
{
	PH_ASSERT_GT(x, 0.0f);

	const float halvedInput = 0.5f * x;

	// gives initial guess for later refinements
	std::uint32_t bits = bitwise_cast<float, std::uint32_t>(x);
	bits = 0x5F375A86 - (bits >> 1);
	x = bitwise_cast<std::uint32_t, float>(bits);

	// Newton's method, each iteration increases accuracy.

	// iteration 1, max. relative error < 0.175125%
	x = x * (1.5f - halvedInput * x * x);

	// iteration 2, disabled since <x> is already good enough
	//x = x * (1.5f - halvedInput * x * x);

	return x;
}

/*! @brief Computes sqrt(x) in a fast but approximative way.
*/
inline float fast_sqrt(float x)
{
	return fast_rcp_sqrt(x) * x;
}

/*! @brief Divide @p numerator by @p denominator and round up to integer.

Both inputs must be a positive integer.
*/
template<typename Integer, typename = std::enable_if_t<std::is_integral_v<Integer>>>
inline Integer ceil_div_positive(const Integer numerator, const Integer denominator)
{
	PH_ASSERT_GE(numerator,   0);
	PH_ASSERT_GT(denominator, 0);

	// check for possible overflow when doing <numerator>+<denominator>
	PH_ASSERT_GE(std::numeric_limits<Integer>::max() - numerator, denominator);

	return (numerator + denominator - 1) / denominator;
}

}// end namespace math

}// end namespace ph