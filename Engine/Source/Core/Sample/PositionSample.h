#pragma once

#include "Math/TVector3.h"

namespace ph
{

class PositionSample final
{
public:
	math::Vector3R position;
	math::Vector3R normal;
	math::Vector3R uvw;
	real           pdf;
};

}// end namespace ph
