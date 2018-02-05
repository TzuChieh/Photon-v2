#pragma once

#include "Common/primitive_type.h"
#include "Math/constant.h"
#include "Math/math_fwd.h"

#include <cmath>
#include <algorithm>
#include <type_traits>

namespace ph
{

class Math final
{
public:
	static const int32 UNKNOWN_AXIS = -1;
	static const int32 X_AXIS       = 0;
	static const int32 Y_AXIS       = 1;
	static const int32 Z_AXIS       = 2;

	static void formOrthonormalBasis(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

	// A fast, without sqrt(), nearly branchless method. Notice that Photon uses y-axis as the up/normal vector. This
	// static method implements the y-is-normal version which is different from the original paper.
	// (Reference: Frisvad, Jeppe Revall, "Building an Orthonormal Basis from a 3D Unit Vector Without Normalization", 
	// Journal of Graphics Tools, 2012)
	// Note: This method seems to have larger numerical error (at least 10^3 larger than my naive method), 
	// thus it is not used currently.
	static void formOrthonormalBasisFrisvad(const Vector3R& unitYaxis, Vector3R* const out_unitXaxis, Vector3R* const out_unitZaxis);

	// Clamp a real value to specific range. If the real value is NaN, its 
	// value is clamped to lower bound. Neither lower bound or upper bound 
	// can be NaN, or the method's behavior is undefined.

	template<typename T, std::enable_if_t<!std::is_floating_point_v<T>, char> = 0>
	static inline T clamp(const T value, const T lowerBound, const T upperBound)
	{
		return std::min(upperBound, std::max(value, lowerBound));
	}

	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, char> = 0>
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
	template<typename T>
	static inline int32 sign(const T value)
	{
		return (static_cast<T>(0) < value) - (static_cast<T>(0) > value);
	}
};

}// end namespace ph