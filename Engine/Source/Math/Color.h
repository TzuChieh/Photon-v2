#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class Color final
{
public:
	static inline float32 linearRgbLuminance(const Vector3f& linearRgb)
	{
		return 0.2126f*linearRgb.x + 0.7152f*linearRgb.y + 0.0722f*linearRgb.z;
	}
};

}// end namespace ph