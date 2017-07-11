#pragma once

#include "Core/BoundingVolume/AABB.h"
#include "Math/TVector3.h"

#include <cstddef>

namespace ph
{

class Intersectable;

class BvhIntersectableInfo final
{
public:
	std::size_t          index;
	AABB                 aabb;
	Vector3R             aabbCentroid;
	const Intersectable* intersectable;

	BvhIntersectableInfo() = default;
	BvhIntersectableInfo(const Intersectable* intersectable, const std::size_t index);
};

}// end namespace ph