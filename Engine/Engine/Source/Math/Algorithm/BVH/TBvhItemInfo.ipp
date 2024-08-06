#pragma once

#include "Math/Algorithm/BVH/TBvhItemInfo.h"

namespace ph::math
{

template<typename Item, typename ItemToAABB>
inline TBvhItemInfo<Item, ItemToAABB>::TBvhItemInfo(
	Item item)

	: aabb        (ItemToAABB{}(item))
	, aabbCentroid(aabb.getCentroid())
	, item        (std::move(item))
{}

}// end namespace ph::math
