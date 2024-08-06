#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/constant.h"
#include "Utility/TSpan.h"

namespace ph::math
{

template<typename Item, typename ItemToAABB>
class TBvhItemInfo;

template<typename Item, typename ItemToAABB>
class TBvhInfoNode final
{
public:
	using ItemInfo = TBvhItemInfo<Item, ItemToAABB>;

	static auto makeBinaryInternal(
		const TBvhInfoNode* child1,
		const TBvhInfoNode* child2,
		std::size_t splitAxis) -> TBvhInfoNode;
	
	static auto makeBinaryLeaf(
		TSpanView<ItemInfo> leafItems,
	    const AABB3D& leafAabb) -> TBvhInfoNode;

public:
	const TBvhInfoNode* children[2];
	TSpanView<ItemInfo> items;
	AABB3D              aabb;
	uint8               splitAxis;

	TBvhInfoNode();

	bool isBinaryLeaf() const;
	bool isBinaryInternal() const;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhInfoNode.ipp"
