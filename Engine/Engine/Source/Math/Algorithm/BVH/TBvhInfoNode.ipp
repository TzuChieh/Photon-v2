#pragma once

#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/utility.h>

namespace ph::math
{

template<std::size_t N, typename Item>
inline auto TBvhInfoNode<N, Item>
::makeInternal(
	const std::array<const TBvhInfoNode*, N>& children,
	std::size_t splitAxis) 
-> TBvhInfoNode
{
	TBvhInfoNode internalNode{};
	internalNode.m_children = children;
	internalNode.m_splitAxis = lossless_cast<decltype(internalNode.m_splitAxis)>(splitAxis);
	internalNode.m_isLeaf = false;

	for(std::size_t ci = 0; ci < children.size(); ++ci)
	{
		if(children[ci])
		{
			internalNode.m_aabb.unionWith(children[ci]->getAABB());
		}
	}

	return internalNode;
}
	
template<std::size_t N, typename Item>
inline auto TBvhInfoNode<N, Item>
::makeLeaf(
	TSpanView<ItemInfoType> leafItems,
	const AABB3D& leafAabb)
-> TBvhInfoNode
{
	TBvhInfoNode leafNode{};
	leafNode.m_items = leafItems;
	leafNode.m_aabb = leafAabb;
	leafNode.m_isLeaf = true;

	return leafNode;
}

template<std::size_t N, typename Item>
inline constexpr bool TBvhInfoNode<N, Item>
::isBinary()
{
	return numChildren() == 2;
}

template<std::size_t N, typename Item>
inline constexpr auto TBvhInfoNode<N, Item>
::numChildren()
-> std::size_t
{
	return N;
}

template<std::size_t N, typename Item>
inline TBvhInfoNode<N, Item>::TBvhInfoNode()
	: m_children{}
	, m_items{}
	, m_aabb(AABB3D::makeEmpty())
	, m_splitAxis(constant::X_AXIS)
	, m_isLeaf(true)
{}

template<std::size_t N, typename Item>
inline bool TBvhInfoNode<N, Item>::isBinaryLeaf() const
{
	if constexpr(!isBinary())
	{
		return false;
	}

	return isLeaf();
}

template<std::size_t N, typename Item>
inline bool TBvhInfoNode<N, Item>
::isBinaryInternal() const
{
	if constexpr(!isBinary())
	{
		return false;
	}

	return !isLeaf();
}

template<std::size_t N, typename Item>
inline bool TBvhInfoNode<N, Item>::isLeaf() const
{
	return m_isLeaf;
}

template<std::size_t N, typename Item>
inline bool TBvhInfoNode<N, Item>
::isInternal() const
{
	return !isLeaf();
}

template<std::size_t N, typename Item>
inline auto TBvhInfoNode<N, Item>
::getChild(const std::size_t childIdx) const
-> const TBvhInfoNode*
{
	PH_ASSERT(isInternal());
	PH_ASSERT_LT(childIdx, m_children.size());

	return m_children[childIdx];
}

template<std::size_t N, typename Item>
inline auto TBvhInfoNode<N, Item>
::getItems() const
-> TSpanView<ItemInfoType>
{
	PH_ASSERT(isLeaf());

	return m_items;
}

template<std::size_t N, typename Item>
inline auto TBvhInfoNode<N, Item>
::getAABB() const
-> const AABB3D&
{
	return m_aabb;
}

template<std::size_t N, typename Item>
inline auto TBvhInfoNode<N, Item>
::getSplitAxis() const
-> std::size_t
{
	PH_ASSERT(isInternal());

	return m_splitAxis;
}

}// end namespace ph::math
