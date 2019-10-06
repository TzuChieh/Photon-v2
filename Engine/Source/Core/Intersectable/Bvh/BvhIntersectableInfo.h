#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"

#include <cstddef>

namespace ph
{

class Intersectable;

class BvhIntersectableInfo final
{
public:
	std::size_t          index;
	math::AABB3D         aabb;
	Vector3R             aabbCentroid;
	const Intersectable* intersectable;

	BvhIntersectableInfo() = default;
	BvhIntersectableInfo(const Intersectable* intersectable, std::size_t index);
};

}// end namespace ph