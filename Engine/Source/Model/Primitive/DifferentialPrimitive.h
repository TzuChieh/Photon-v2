#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class DifferentialPrimitive final
{
public:
	float32 u;
	float32 v;
	Vector3f dndu;
	Vector3f dndv;
};

}// end namespace ph