#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace ph::math
{

template<typename Item, typename ItemToAABB>
class TBvhItemInfo final
{
	static_assert(std::is_invocable_r_v<AABB3D, ItemToAABB, Item>);

public:
	AABB3D      aabb;
	Vector3R    aabbCentroid;
	Item        item;

	TBvhItemInfo() = default;
	explicit TBvhItemInfo(Item item);
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhItemInfo.ipp"
