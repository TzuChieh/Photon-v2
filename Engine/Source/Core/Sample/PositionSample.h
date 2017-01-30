#pragma once

#include "Math/TVector3.h"

namespace ph
{

class PositionSample final
{
public:
	Vector3R position;
	Vector3R normal;
	Vector3R uvw;
	float32  pdf;
};

}// end namespace ph