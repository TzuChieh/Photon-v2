#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class SenseEvent final
{
public:
	float32 filmX;
	float32 filmY;
	Vector3R radiance;

	SenseEvent(const float32 filmX, const float32 filmY, const Vector3R& radiance) :
		filmX(filmX), filmY(filmY), radiance(radiance)
	{

	}
};

}// end namespace ph