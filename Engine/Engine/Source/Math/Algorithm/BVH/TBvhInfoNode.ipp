#pragma once

#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/utility.h>

namespace ph::math
{

template<typename Item>
inline auto TBvhInfoNode<Item>::makeBinaryInternal(
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

template<typename Item>
inline auto TBvhInfoNode<Item>::makeBinaryLeaf(
	TSpanView<ItemInfo> leafItems,
	const AABB3D& leafAabb)
-> TBvhInfoNode
{
	TBvhInfoNode leafNode;
	leafNode.aabb  = leafAabb;
	leafNode.items = leafItems;

	return leafNode;
}

template<typename Item>
inline TBvhInfoNode<Item>::TBvhInfoNode()
	: children{}
	, items()
	, aabb(AABB3D::makeEmpty())
	, splitAxis(-1)
{}

template<typename Item>
inline bool TBvhInfoNode<Item>::isBinaryLeaf() const
{
	return !children[0] && !children[1] && !items.empty();
}

template<typename Item>
inline bool TBvhInfoNode<Item>::isBinaryInternal() const
{
	return children[0] && children[1] && items.empty();
}

}// end namespace ph::math
