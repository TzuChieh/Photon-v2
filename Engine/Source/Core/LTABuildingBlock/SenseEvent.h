#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class SenseEvent final
{
public:
	//real filmX;
	//real filmY;
	SpectralStrength radiance;

	SenseEvent(/*const real filmX, const real filmY, */const SpectralStrength& radiance) :
		/*filmX(filmX), filmY(filmY), */radiance(radiance)
	{}
};

}// end namespace ph
