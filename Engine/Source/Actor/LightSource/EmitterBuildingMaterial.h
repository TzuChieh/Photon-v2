#pragma once

#include "Math/Transform.h"

#include <vector>

namespace ph
{

class Primitive;

class EmitterBuildingMaterial final
{
public:
	std::vector<const Primitive*> primitives;
	Transform localToWorld;
	Transform worldToLocal;

	inline EmitterBuildingMaterial() : 
		primitives(), localToWorld(), worldToLocal()
	{

	}
};

}// end namespace ph