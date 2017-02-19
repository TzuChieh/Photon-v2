#pragma once

#include "Core/BoundingVolume/AABB.h"
#include "Math/TVector3.h"

#include <cstddef>

namespace ph
{

class Primitive;

class BvhPrimitiveInfo final
{
public:
	std::size_t      index;
	AABB             aabb;
	Vector3R         aabbCentroid;
	const Primitive* primitive;

	BvhPrimitiveInfo() = default;
	BvhPrimitiveInfo(const Primitive* primitive, const std::size_t index);
};

}// end namespace ph