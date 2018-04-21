#pragma once

#include "Math/Transform/RigidTransform.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;
class PrimitiveMetadata;

class EmitterBuildingMaterial final
{
public:
	std::vector<const Primitive*> primitives;
	PrimitiveMetadata*            metadata;

	inline EmitterBuildingMaterial() : 
		primitives(),
		metadata(nullptr)
	{}
};

}// end namespace ph