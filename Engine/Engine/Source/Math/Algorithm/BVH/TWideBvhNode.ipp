#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"

namespace ph::math
{

template<std::size_t N, typename Index>
inline TWideBvhNode<N, Index>
::TWideBvhNode()
	: m_aabbMins{}
	, m_aabbMaxs{}
	, m_offsets(make_array<Index, N>(static_cast<Index>(-1)))
	, m_childrenData{}
{
	const auto emptyAABB = AABB3D::makeEmpty();
	for(std::size_t di = 0; di < 3; ++di)
	{
		for(std::size_t ni = 0; ni < N; ++ni)
		{
			m_aabbMins[di][ni] = emptyAABB.getMinVertex()[di];
			m_aabbMaxs[di][ni] = emptyAABB.getMaxVertex()[di];
		}
	}
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::getAABB(const std::size_t childIdx) const
-> AABB3D
{
	PH_ASSERT_LT(childIdx, N);

	return AABB3D(
		Vector3R(m_aabbMins[0][childIdx], m_aabbMins[1][childIdx], m_aabbMins[2][childIdx]),
		Vector3R(m_aabbMaxs[0][childIdx], m_aabbMaxs[1][childIdx], m_aabbMaxs[2][childIdx]));
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::getMinVerticesOnAxis(const std::size_t axis) const
-> TSpanView<real, N>
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(axis, 0, 2);

	return m_aabbMins[axis];
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::getMaxVerticesOnAxis(const std::size_t axis) const
-> TSpanView<real, N>
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(axis, 0, 2);

	return m_aabbMaxs[axis];
}

template<std::size_t N, typename Index>
inline bool TWideBvhNode<N, Index>
::isLeaf(const std::size_t childIdx) const
{
	PH_ASSERT_LT(childIdx, m_childrenData.size());

	return m_childrenData[childIdx].isLeaf;
}

template<std::size_t N, typename Index>
inline bool TWideBvhNode<N, Index>
::isInternal(const std::size_t childIdx) const
{
	return !isLeaf(childIdx);
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::getChildOffset(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isInternal(childIdx));

	return static_cast<std::size_t>(m_offsets[childIdx]);
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::getSplitAxis(const std::size_t childIdx) const
-> std::size_t
{
	return static_cast<std::size_t>(m_childrenData[childIdx].splitAxis);
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::getItemOffset(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isLeaf(childIdx));

	return static_cast<std::size_t>(m_offsets[childIdx]);
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::numItems(const std::size_t childIdx) const
-> std::size_t
{
	PH_ASSERT(isLeaf(childIdx));

	return static_cast<std::size_t>(m_childrenData[childIdx].numItems);
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::setInternal(
	const std::size_t childIdx,
	const AABB3D& childAABB,
	const std::size_t childOffset,
	const std::size_t splitAxis)
-> TWideBvhNode&
{
	PH_ASSERT_LT(childIdx, N);
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxis, 0, 2);

	for(std::size_t di = 0; di < 3; ++di)
	{
		m_aabbMins[di][childIdx] = childAABB.getMinVertex()[di];
		m_aabbMaxs[di][childIdx] = childAABB.getMaxVertex()[di];
	}
	
	m_offsets[childIdx] = lossless_cast<Index>(childOffset);
	m_childrenData[childIdx] = {
		.isLeaf = false,
		.splitAxis = static_cast<uint8>(splitAxis),
		.numItems = 0};

	return *this;
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::setLeaf(
	const std::size_t childIdx,
	const AABB3D& childAABB,
	const std::size_t itemOffset,
	const std::size_t splitAxis,
	const std::size_t numItems)
-> TWideBvhNode&
{
	PH_ASSERT_LT(childIdx, N);
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxis, 0, 2);
	PH_ASSERT_LE(numItems, MAX_NODE_ITEMS);

	for(std::size_t di = 0; di < 3; ++di)
	{
		m_aabbMins[di][childIdx] = childAABB.getMinVertex()[di];
		m_aabbMaxs[di][childIdx] = childAABB.getMaxVertex()[di];
	}

	m_offsets[childIdx] = lossless_cast<Index>(itemOffset);
	m_childrenData[childIdx] = {
		.isLeaf = true,
		.splitAxis = static_cast<uint8>(splitAxis),
		.numItems = static_cast<uint8>(numItems)};

	return *this;
}

template<std::size_t N, typename Index>
inline auto TWideBvhNode<N, Index>
::setEmptyLeaf(
	const std::size_t childIdx,
	const std::size_t splitAxis)
-> TWideBvhNode&
{
	return setLeaf(
		childIdx,
		AABB3D::makeEmpty(),
		static_cast<Index>(-1),
		splitAxis,
		0);
}

}// end namespace ph::math
