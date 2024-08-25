#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"

namespace ph::math
{

template<std::size_t N, typename Item, typename Index>
inline TWideBvhNode<N, Item, Index>
::TWideBvhNode()
	: m_aabbs(make_array<AABB3D, 4>(AABB3D::makeEmpty()))
	, m_offsets(make_array<Index, 4>(static_cast<Index>(-1)))
	, m_childrenData{}
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
	PH_ASSERT_LT(childIdx, m_childrenData.size());

	return m_childrenData[childIdx].isLeaf;
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

	return static_cast<std::size_t>(m_offsets[childIdx]);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::getSplitAxis(const std::size_t childIdx) const
-> std::size_t
{
	return static_cast<std::size_t>(m_childrenData[childIdx].splitAxis);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::getItemOffset(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isLeaf(childIdx));

	return static_cast<std::size_t>(m_offsets[childIdx]);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::numItems(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isLeaf(childIdx));

	return static_cast<std::size_t>(m_childrenData[childIdx].numItems);
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::setInternal(
	const std::size_t childIdx,
	const AABB3D& childAABB,
	const std::size_t childOffset,
	const std::size_t splitAxis)
-> TWideBvhNode&
{
	PH_ASSERT_LT(childIdx, m_aabbs.size());
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxis, 0, 2);

	m_aabbs[childIdx] = childAABB;
	m_offsets[childIdx] = lossless_cast<Index>(childOffset);
	m_childrenData[childIdx] = {
		.isLeaf = false,
		.splitAxis = static_cast<uint8>(splitAxis),
		.numItems = 0};

	return *this;
}

template<std::size_t N, typename Item, typename Index>
inline auto TWideBvhNode<N, Item, Index>
::setLeaf(
	const std::size_t childIdx,
	const AABB3D& childAABB,
	const std::size_t itemOffset,
	const std::size_t splitAxis,
	const std::size_t numItems)
-> TWideBvhNode&
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxis, 0, 2);
	PH_ASSERT_LE(numItems, MAX_NODE_ITEMS);

	m_aabbs[childIdx] = childAABB;
	m_offsets[childIdx] = lossless_cast<Index>(itemOffset);
	m_childrenData[childIdx] = {
		.isLeaf = true,
		.splitAxis = static_cast<uint8>(splitAxis),
		.numItems = static_cast<uint8>(numItems)};

	return *this;
}

}// end namespace ph::math
