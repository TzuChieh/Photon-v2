#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class SenseEvent final
{
public:
	//real filmX;
	//real filmY;
	Vector3R radiance;

	SenseEvent(/*const real filmX, const real filmY, */const Vector3R& radiance) :
		/*filmX(filmX), filmY(filmY), */radiance(radiance)
	{

	}
};

}// end namespace ph