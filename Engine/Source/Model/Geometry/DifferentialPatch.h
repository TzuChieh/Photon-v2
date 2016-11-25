#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class DifferentialPatch final
{
public:
	float32 u;
	float32 v;
	Vector3f dpdu;
	Vector3f dpdv;
};

}// end namespace ph