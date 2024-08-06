#pragma once

#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/math.h"
#include "Utility/utility.h"

#include <Common/assertion.h>

namespace ph::math
{

template<typename Item, typename ItemToAABB>
inline auto TBvhInfoNode<Item, ItemToAABB>::makeBinaryInternal(
	const TBvhInfoNode* child1,
	const TBvhInfoNode* child2, 
	const std::size_t splitAxis)
-> TBvhInfoNode
{
	PH_ASSERT(child1);
	PH_ASSERT(child2);

	TBvhInfoNode internalNode;
	internalNode.children[0] = child1;
	internalNode.children[1] = child2;
	internalNode.aabb        = AABB3D::makeUnioned(child1->aabb, child2->aabb);
	internalNode.splitAxis   = lossless_cast<decltype(internalNode.splitAxis)>(splitAxis);

	return internalNode;
}

template<typename Item, typename ItemToAABB>
inline auto TBvhInfoNode<Item, ItemToAABB>::makeBinaryLeaf(
	TSpanView<ItemInfo> leafItems,
	const AABB3D& leafAabb)
-> TBvhInfoNode
{
	TBvhInfoNode leafNode;
	leafNode.aabb  = leafAabb;
	leafNode.items = leafItems;

	return leafNode;
}

template<typename Item, typename ItemToAABB>
inline TBvhInfoNode<Item, ItemToAABB>::TBvhInfoNode()
	: children{}
	, items()
	, aabb()
	, splitAxis(-1)
{}

template<typename Item, typename ItemToAABB>
inline bool TBvhInfoNode<Item, ItemToAABB>::isBinaryLeaf() const
{
	return !children[0] && !children[1] && !items.empty();
}

template<typename Item, typename ItemToAABB>
inline bool TBvhInfoNode<Item, ItemToAABB>::isBinaryInternal() const
{
	return children[0] && children[1] && items.empty();
}

}// end namespace ph::math
