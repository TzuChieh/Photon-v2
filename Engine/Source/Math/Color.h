#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph::math
{

class Color final
{
public:
	static inline real linearRgbLuminance(const Vector3R& linearRgb)
	{
		return 0.2126_r * linearRgb.x + 0.7152_r * linearRgb.y + 0.0722_r * linearRgb.z;
	}
};

}// end namespace ph::math
