#pragma once

#include "Math/Algorithm/BVH/TBinaryBvhNode.h"
#include "Math/constant.h"

namespace ph::math
{

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::makeInternal(
	const AABB3D& nodeAABB,
	const std::size_t childOffset,
	const std::size_t splitAxis)
-> TBinaryBvhNode
{
	static_assert(constant::X_AXIS == X_AXIS_FLAG);
	static_assert(constant::Y_AXIS == Y_AXIS_FLAG);
	static_assert(constant::Z_AXIS == Z_AXIS_FLAG);

	PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxis, 0, 2);

	TBinaryBvhNode node;
	node.m_aabb             = nodeAABB;
	node.u0_childOffset     = lossless_cast<decltype(node.u0_childOffset)>(childOffset);
	node.m_numItemsAndFlags = static_cast<uint8>(splitAxis);

	return node;
}

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::makeLeaf(
	const AABB3D& nodeAABB,
	const std::size_t itemOffset,
	const std::size_t numItems)
-> TBinaryBvhNode
{
	PH_ASSERT_LE(numItems, MAX_NODE_ITEMS);

	TBinaryBvhNode node;
	node.m_aabb             = nodeAABB;
	node.u0_itemOffset      = lossless_cast<decltype(node.u0_itemOffset)>(itemOffset);
	node.m_numItemsAndFlags = static_cast<uint8>((numItems << NUM_FLAG_BITS) | LEAF_FLAG);

	return node;
}

template<typename Item, typename Index>
inline TBinaryBvhNode<Item, Index>
::TBinaryBvhNode()
	: m_aabb(AABB3D::makeEmpty())
	, u0_itemOffset(static_cast<decltype(u0_itemOffset)>(-1))
	, m_numItemsAndFlags(LEAF_FLAG)// leaf with 0 item
{}

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::getAABB() const
-> const AABB3D&
{
	return m_aabb;
}

template<typename Item, typename Index>
inline bool TBinaryBvhNode<Item, Index>
::isLeaf() const
{
	return (m_numItemsAndFlags & FLAG_BITS_MASK) == LEAF_FLAG;
}

template<typename Item, typename Index>
inline bool TBinaryBvhNode<Item, Index>
::isInternal() const
{
	return !isLeaf();
}

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::getChildOffset() const
-> std::size_t
{
	PH_ASSERT(isInternal());

	return static_cast<std::size_t>(u0_childOffset);
}

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::getSplitAxis() const
-> std::size_t
{
	PH_ASSERT(isInternal());

	return static_cast<std::size_t>(m_numItemsAndFlags & FLAG_BITS_MASK);
}

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::getItemOffset() const
-> std::size_t
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(u0_itemOffset);
}

template<typename Item, typename Index>
inline auto TBinaryBvhNode<Item, Index>
::numItems() const
-> std::size_t
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(m_numItemsAndFlags >> NUM_FLAG_BITS);
}

}// end namespace ph::math
