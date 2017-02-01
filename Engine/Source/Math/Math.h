#pragma once

#include "Common/primitive_type.h"
#include "Math/constant.h"

#include <cmath>
#include <algorithm>

namespace ph
{

class Math final
{
public:

	// Clamp a real value to specific range. If the real value is NaN, its value is clamped to lower bound. 
	// Either lower bound or upper bound shall not be NaN, or the method's behavior is undefined.

	static inline real clamp(const real value, const real lowerBound, const real upperBound)
	{
		return fmin(upperBound, fmax(value, lowerBound));
	}

	static inline int32 clamp(const int32 value, const int32 lowerBound, const int32 upperBound)
	{
		return std::min(upperBound, std::max(value, lowerBound));
	}

	static inline real toDegrees(const real radians)
	{
		return radians * RECI_PI_REAL * 180.0_r;
	}

	static inline real toRadians(const real degrees)
	{
		return degrees * (1.0_r / 180.0_r) * PI_REAL;
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