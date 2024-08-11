#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"
#include "Math/constant.h"

namespace ph::math
{

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::makeInternal(
	const std::array<AABB3D, N>& childrenAABBs,
	const std::array<std::size_t, N - 1>& childOffsets,
	const std::array<std::size_t, N>& splitAxes)
-> TWideBvhNode
{
	static_assert(constant::X_AXIS == X_AXIS_FLAG);
	static_assert(constant::Y_AXIS == Y_AXIS_FLAG);
	static_assert(constant::Z_AXIS == Z_AXIS_FLAG);

	TWideBvhNode node;
	node.m_aabbs = childrenAABBs;

	for(std::size_t ni = 0; ni < childOffsets.size(); ++ni)
	{
		node.u0_childOffsets[ni] = lossless_cast<decltype(node.u0_childOffsets[ni])>(
			childOffsets[ni]);
	}

	for(std::size_t ni = 0; ni < splitAxes.size(); ++ni)
	{
		PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxes[ni], 0, 2);

		node.m_numItemsAndFlags[ni] = lossless_cast<decltype(node.m_numItemsAndFlags[ni])>(
			splitAxes[ni]);
	}

	return node;
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::makeLeaf(
	const std::array<AABB3D, N>& childrenAABBs,
	const std::array<std::size_t, N> itemOffsets,
	const std::array<std::size_t, N> numItems)
-> TWideBvhNode
{
	PH_ASSERT_LE(numItems, MAX_NODE_ITEMS);

	TWideBvhNode node;
	node.m_aabbs = childrenAABBs;

	for(std::size_t ni = 0; ni < itemOffsets.size(); ++ni)
	{
		node.u0_itemOffsets[ni] = lossless_cast<decltype(node.u0_itemOffsets[ni])>(
			itemOffsets[ni]);
	}
	
	for(std::size_t ni = 0; ni < numItems.size(); ++ni)
	{
		node.m_numItemsAndFlags[ni] = static_cast<uint8>(
			(numItems[ni] << NUM_FLAG_BITS) | LEAF_FLAG);
	}

	return node;
}

template<std::size_t N, typename Item, typename Index>
inline TWideBvhNode<N, Item, Index>
::TWideBvhNode()
	: m_aabbs(AABB3D::makeEmpty())
	, u0_itemOffsets(make_array<Index, 4>(static_cast<decltype(u0_itemOffsets[0])>(-1)))
	, m_numItemsAndFlags(static_cast<decltype(m_numItemsAndFlags[0])>(-1))
{}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::getAABB(const std::size_t childIdx) const
-> const AABB3D&
{
	PH_ASSERT_LT(childIdx, m_aabbs.size());

	return m_aabbs[childIdx];
}

template<std::size_t N, typename Item, typename Index>
inline bool TWideBvhNode<N, Item, Index>
::isLeaf(const std::size_t childIdx) const
{
	PH_ASSERT_LT(childIdx, m_numItemsAndFlags.size());

	return (m_numItemsAndFlags[childIdx] & FLAG_BITS_MASK) == LEAF_FLAG;
}

template<std::size_t N, typename Item, typename Index>
inline bool TWideBvhNode<N, Item, Index>
::isInternal(const std::size_t childIdx) const
{
	return !isLeaf(childIdx);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::getChildOffset(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isInternal(childIdx));

	// Not storing the first child offset
	PH_ASSERT_LT(childIdx, u0_childOffsets.size());

	return static_cast<std::size_t>(u0_childOffsets[childIdx]);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::getSplitAxis(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isInternal(childIdx));

	return static_cast<std::size_t>(m_numItemsAndFlags[childIdx] & FLAG_BITS_MASK);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::getItemOffset(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isLeaf(childIdx));

	return static_cast<std::size_t>(u0_itemOffsets[childIdx]);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::numItems(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isLeaf(childIdx));

	return static_cast<std::size_t>(m_numItemsAndFlags[childIdx] >> NUM_FLAG_BITS);
}

}// end namespace ph::math
