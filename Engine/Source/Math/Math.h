#pragma once

#include "Common/primitive_type.h"

#include <cmath>

namespace ph
{

class Math final
{
public:

	// Clamp current value to specific range. If the value is NaN, its value is clamped to lower bound. 
	// Either lower bound or upper bound shall not be NaN, or the method's behavior is undefined.

	static inline float32 clamp(const float32 value, const float32 lowerBound, const float32 upperBound)
	{
		return fmin(upperBound, fmax(value, lowerBound));
	}
};

}// end namespace ph