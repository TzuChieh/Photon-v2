#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class PositionSample final
{
public:
	Vector3f position;
	Vector3f normal;
	Vector3f uvw;
	float32  pdf;
};

}// end namespace ph