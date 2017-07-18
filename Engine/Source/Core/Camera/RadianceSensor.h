#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class RadianceSensor final
{
public:
	float64 accuR;
	float64 accuG;
	float64 accuB;
	float64 accuWeight;

	RadianceSensor() : 
		accuR(0), accuG(0), accuB(0), accuWeight(0)
	{

	}
};

}// end namespace ph