#pragma once

#include "Common/primitive_type.h"
#include "Math/constant.h"
#include "Math/math_fwd.h"
#include "Common/compiler.h"
#include "Common/assertion.h"

#include <cmath>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <limits>
#include <array>

#if defined(PH_COMPILER_IS_MSVC)
	#include <intrin.h>
#endif

namespace ph
{

class Math final
{
public:
	static const int32 UNKNOWN_AXIS = -1;
	static const int32 X_AXIS       = 0;
	static const int32 Y_AXIS       = 1;
	static const int32 Z_AXIS       = 2;

	template<typename T>
	static inline auto matrix2x2(const T e00, const T e01, const T e10, const T e11)
		-> std::array<std::array<T, 2>, 2>
	{
		return
		{{
			{{e00, e01}},
			{{e10, e11}}
		}};
	}

	template<typename T>
	static inline T squared(const T value)
	{
		return value * value;
	}

	static void formOrthonormalBasis(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

	// A fast, without sqrt(), nearly branchless method. Notice that Photon uses y-axis as the up/normal vector. This
	// static method implements the y-is-normal version which is different from the original paper.
	// (Reference: Frisvad, Jeppe Revall, "Building an Orthonormal Basis from a 3D Unit Vector Without Normalization", 
	// Journal of Graphics Tools, 2012)
	// Note: This method seems to have larger numerical error (at least 10^3 larger than my naive method), 
	// thus it is not used currently.
	//
	static void formOrthonormalBasisFrisvad(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

	// Clamp a real value to specific range. If the real value is NaN, its 
	// value is clamped to lower bound. Neither lower bound or upper bound 
	// can be NaN, or the method's behavior is undefined.

	template<typename T, std::enable_if_t<!std::is_floating_point_v<T>, int> = 0>
	static inline T clamp(const T value, const T lowerBound, const T upperBound)
	{
		return std::min(upperBound, std::max(value, lowerBound));
	}

	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	static inline T clamp(const T value, const T lowerBound, const T upperBound)
	{
		return std::fmin(upperBound, std::fmax(value, lowerBound));
	}

	template<typename T>
	static inline T toDegrees(const T radians)
	{
		return radians * static_cast<T>(PH_RECI_PI * 180.0);
	}

	template<typename T>
	static inline T toRadians(const T degrees)
	{
		return degrees * static_cast<T>(PH_PI / 180.0);
	}

	// Extract the sign of a value (branchless).
	// If the provided value is comparable to 0:
	// returns ( 1) when (value  > 0), 
	// returns (-1) when (value  < 0), 
	// returns ( 0) when (value == 0).
	// (Note that the target value is pass by value.)
	//
	template<typename T>
	static inline int sign(const T value)
	{
		return (static_cast<T>(0) < value) - (static_cast<T>(0) > value);
	}

	// Returns the a power of 2 value that is >= <value>. Note that if 
	// <value> is 0, then 0 will be returned.
	//
	// Reference:
	// Stanford CG Lab's webpage: "Bit Twiddling Hacks" by Sean Eron Anderson
	//
	static inline uint32 nextPowerOf2(uint32 value)
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

	template<typename T>
	static inline bool isPowerOf2(const T value)
	{
		return (value > 0) && !(value & (value - 1));
	}

	// Calculate a positive integer's base 2 logarithm (floored). 
	// <input> shall not be 0, or the behavior of this method is undefined.
	//
	static inline uint32 log2Floor(const uint32 value)
	{
		PH_ASSERT(value != 0);

#if defined(PH_COMPILER_IS_CLANG) || defined(PH_COMPILER_IS_GCC)

		static_assert(sizeof(uint32) == sizeof(unsigned int), 
		              "expecting same size for conversion purposes");

		const int numLeftZeros = __builtin_clz(value);
		PH_ASSERT(numLeftZeros >= 0);

		return 31 - static_cast<uint32>(numLeftZeros);

#elif defined(PH_COMPILER_IS_MSVC)

		static_assert(sizeof(uint32) == sizeof(unsigned long), 
		              "expecting same size for conversion purposes");

		unsigned long first1BitFromLeftIndex;
		_BitScanReverse(&first1BitFromLeftIndex, value);
		return first1BitFromLeftIndex;

#else

		return static_cast<uint32>(std::log2(static_cast<float>(value)));

#endif
	}// end log2Floor(1)

	// Retrieve the fractional part of <value> (with the same sign 
	// as <value>). The result is not guaranteed to be the same as the bit 
	// representation of <value>'s fractional part.
	// The result is undefined if input value is NaN or +-Inf.
	//
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	static inline T fractionalPart(const T value)
	{
		long double integralPart;
		return static_cast<T>(std::modf(static_cast<long double>(value), &integralPart));
	}

	// Solves Ax = b where A is a 2x2 matrix and x & b are 2x1 vectors. If x
	// is successfully solved, method returns true and <out_x> stores the 
	// answer; otherwise, false is returned and what <out_x> stores is undefined.
	//
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	static inline bool solveLinearSystem2x2(
		const std::array<std::array<T, 2>, 2>& A,
		const std::array<T, 2>& b,
		std::array<T, 2>* const out_x)
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

	// Wraps an integer around [lower-bound, upper-bound]. 
	// For example, given a bound [-1, 2], 3 will be wrapped to -1.
	//
	template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	static inline T wrap(T value, const T lowerBound, const T upperBound)
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
	static bool isSameHemisphere(const Vector3R& vector, const Vector3R& N);
};

}// end namespace ph