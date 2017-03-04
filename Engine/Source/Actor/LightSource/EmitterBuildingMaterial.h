#pragma once

#include <vector>

namespace ph
{

class Primitive;

class EmitterBuildingMaterial final
{
public:
	std::vector<const Primitive*> primitives;

	inline EmitterBuildingMaterial() : 
		primitives()
	{

	}
};

}// end namespace ph