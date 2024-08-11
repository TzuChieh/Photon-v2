#pragma once

#include "Math/Algorithm/BVH/TBvhItemInfo.h"

#include <utility>

namespace ph::math
{

template<typename Item>
inline TBvhItemInfo<Item>::TBvhItemInfo(
	const AABB3D& aabb, Item item)

	: aabb        (aabb)
	, aabbCentroid(aabb.getCentroid())
	, item        (std::move(item))
{}

}// end namespace ph::math
