#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class SenseEvent final
{
public:
	float32 filmX;
	float32 filmY;
	Vector3f radiance;

	SenseEvent(const float32 filmX, const float32 filmY, const Vector3f& radiance) : 
		filmX(filmX), filmY(filmY), radiance(radiance)
	{

	}
};

}// end namespace ph