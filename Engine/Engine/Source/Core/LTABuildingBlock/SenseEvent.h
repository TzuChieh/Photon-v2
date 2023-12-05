#pragma once

#include "Math/TVector3.h"
#include "Core/Quantity/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class SenseEvent final
{
public:
	//real filmX;
	//real filmY;
	Spectrum radiance;

	SenseEvent(/*const real filmX, const real filmY, */const Spectrum& radiance) :
		/*filmX(filmX), filmY(filmY), */radiance(radiance)
	{}
};

}// end namespace ph
