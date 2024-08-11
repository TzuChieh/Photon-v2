#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"

namespace ph::math
{

template<typename Item>
class TBvhItemInfo final
{
public:
	AABB3D      aabb;
	Vector3R    aabbCentroid;
	Item        item;

	TBvhItemInfo() = default;
	TBvhItemInfo(const AABB3D& aabb, Item item);
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhItemInfo.ipp"
