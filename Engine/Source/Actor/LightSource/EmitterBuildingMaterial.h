#pragma once

#include "Math/Transform/RigidTransform.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;

class EmitterBuildingMaterial final
{
public:
	std::vector<const Primitive*>   primitives;
	std::unique_ptr<RigidTransform> baseLocalToWorld;
	std::unique_ptr<RigidTransform> baseWorldToLocal;

	inline EmitterBuildingMaterial() : 
		primitives(),
		baseLocalToWorld(nullptr),
		baseWorldToLocal(nullptr)
	{}
};

}// end namespace ph