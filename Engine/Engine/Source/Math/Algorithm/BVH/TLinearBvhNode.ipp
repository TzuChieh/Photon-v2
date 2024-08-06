#pragma once

#include "Math/Algorithm/BVH/TLinearBvhNode.h"
#include "Utility/utility.h"

namespace ph::math
{

template<typename Item>
inline auto TLinearBvhNode<Item>::makeInternal(
	const AABB3D& nodeAABB,
	const std::size_t secondChildOffset,
	const std::size_t splitAxis)
-> TLinearBvhNode
{
	TLinearBvhNode node;
	node.aabb              = nodeAABB;
	node.secondChildOffset = secondChildOffset;
	node.splitAxis         = lossless_cast<decltype(node.splitAxis)>(splitAxis);
	node.m_isLeaf          = false;

	return node;
}

template<typename Item>
inline auto TLinearBvhNode<Item>::makeLeaf(
	const AABB3D& nodeAABB,
	const std::size_t itemOffset,
	const std::size_t numPrimitives)
-> TLinearBvhNode
{
	TLinearBvhNode node;
	node.aabb       = nodeAABB;
	node.itemOffset = itemOffset;
	node.numItems   = lossless_cast<decltype(node.numItems)>(numPrimitives);
	node.m_isLeaf   = true;

	return node;
}

template<typename Item>
inline TLinearBvhNode<Item>::TLinearBvhNode()
	: aabb()
	, secondChildOffset(static_cast<decltype(secondChildOffset)>(-1))
	, splitAxis(static_cast<decltype(splitAxis)>(-1))
	, m_isLeaf(false)
{}

template<typename Item>
inline bool TLinearBvhNode<Item>::isLeaf() const
{
	return m_isLeaf;
}

template<typename Item>
inline bool TLinearBvhNode<Item>::isInternal() const
{
	return !m_isLeaf;
}

}// end namespace ph::math
