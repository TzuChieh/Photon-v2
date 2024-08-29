#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TBvhItemInfo.h"
#include "Math/Algorithm/BVH/TBvhSseF32ComputingContext.h"
#include "Math/Algorithm/acceleration_structure_basics.h"
#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

#include <type_traits>
#include <utility>
#include <limits>

namespace ph::math
{

template<std::size_t N, typename Item, typename Index>
template<std::size_t SrcN>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::build(
	const TBvhInfoNode<SrcN, Item>* const rootNode,
	const std::size_t totalInfoNodes,
	const std::size_t totalItems)
{
#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	*this = TLinearDepthFirstWideBvh{};
	if(!rootNode)
	{
		return;
	}

	// Allocate memory for nodes and items
	m_nodes = std::make_unique<TWideBvhNode<N, Index>[]>(totalInfoNodes);
	m_items = std::make_unique<Item[]>(totalItems);

	// Can directly convert if the branch factor matches
	if constexpr(SrcN == N)
	{
		// Will be updated by the following method if needed
		m_isSingleSplitAxisNodes = true;

		if(rootNode->isLeaf())
		{
			const auto itemOffset = m_numItems;
			for(std::size_t i = 0; i < rootNode->getItems().size(); ++i)
			{
				m_items[itemOffset + i] = rootNode->getItems()[i].item;
			}
			m_numItems += static_cast<Index>(rootNode->getItems().size());

			// Does not matter as there is only a single child
			constexpr std::size_t singleLeafSplitAxis = constant::X_AXIS;

			m_numNodes = 1;
			m_nodes[0].setLeaf(
				0,
				rootNode->getAABB(),
				itemOffset,
				singleLeafSplitAxis,
				rootNode->getItems().size());
		}
		else
		{
			convertChildNodesRecursive(rootNode);
		}
	}
	// Try to collapse into target branch factor
	else
	{
		// TODO
	}

	refitBuffer(totalInfoNodes, totalItems);
}

template<std::size_t N, typename Item, typename Index>
template<bool IS_ROBUST, typename TesterFunc>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const
{
	static_assert(CItemSegmentIntersectionTester<TesterFunc, Item>);
	static_assert(std::numeric_limits<real>::has_infinity);

#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	if(isEmpty())
	{
		return false;
	}

	PH_ASSERT(m_nodes);
	PH_ASSERT(m_items);

	if(m_isCollapsedNodes)
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
	else
	{
		if(m_isSingleSplitAxisNodes)
		{
			return nearestTraversalNaive<IS_ROBUST, true>(
				segment,
				std::forward<TesterFunc>(intersectionTester));
		}
		else
		{
			return nearestTraversalNaive<IS_ROBUST, false>(
				segment,
				std::forward<TesterFunc>(intersectionTester));
		}
	}
}

template<std::size_t N, typename Item, typename Index>
template<bool IS_ROBUST, bool IS_SINGLE_SPLIT_AXIS, typename TesterFunc>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::nearestTraversalNaive(
	const TLineSegment<real>& segment,
	TesterFunc&& intersectionTester) const
{
#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	// Traversal states
	TArrayStack<Index, TRAVERSAL_STACK_SIZE> todoNodes;
	Index currentNodeIndex = 0;
	TLineSegment<real> longestSegment(segment);
	bool hasHit = false;

	// Precompute common values

	//constexpr auto singleSplitAxisOrderTable = makeSingleSplitAxisOrderTable();
	constexpr auto largestHitT = std::numeric_limits<real>::infinity();

	const auto rcpSegmentDir = segment.getDir().rcp();

	std::array<bool, 3> isNegDir;
	if constexpr(IS_SINGLE_SPLIT_AXIS)
	{
		isNegDir = {
			segment.getDir().x() < 0,
			segment.getDir().y() < 0,
			segment.getDir().z() < 0};
	}

	TBvhSseF32ComputingContext<N, Index> sseCtx;
	if constexpr(sseCtx.isSupported())
	{
		sseCtx.setSegment(segment.getOrigin(), rcpSegmentDir);
	}

	// Traverse nodes
	while(true)
	{
#if PH_PROFILE_ACCELERATION_STRUCTURES
		PH_PROFILE_NAMED_SCOPE("Traversal loop body");
#endif

		PH_ASSERT_LT(currentNodeIndex, m_numNodes);
		const NodeType& node = m_nodes[currentNodeIndex];

		//uint32f sseHitMask = 0;
		decltype(sseCtx.getIntersectResultAsMinTsOr(0)) sseHitTs;
		if constexpr(sseCtx.isSupported())
		{
#if PH_PROFILE_ACCELERATION_STRUCTURES
			PH_PROFILE_NAMED_SCOPE("SSE batch AABB intersection");
#endif

			sseCtx.setNode(node);
			sseCtx.intersectAabbVolumes(longestSegment.getMinT(), longestSegment.getMaxT());
			sseHitTs = sseCtx.getIntersectResultAsMinTsOr(largestHitT);
			//sseHitMask = sseCtx.getIntersectResultAsMask();
		}

		std::array<real, N> hitTs;
		if constexpr(!sseCtx.isSupported())
		{
#if PH_PROFILE_ACCELERATION_STRUCTURES
			PH_PROFILE_NAMED_SCOPE("Batch AABB intersection");
#endif

			for(std::size_t i = 0; i < N; ++i)
			{
				const auto [aabbMinT, aabbMaxT] = node.getAABB(i).isIntersectingVolume<IS_ROBUST>(
					longestSegment, rcpSegmentDir);
				hitTs[i] = aabbMinT <= aabbMaxT ? aabbMinT : largestHitT;
			}
		}

		std::array<Index, N> nextChildNodes;
		std::size_t numNextChildNodes = 0;
		for(std::size_t i = 0; i < N; ++i)
		{
			// If all children have the same split axis, traverse from the nearest one
			std::size_t ci = i;
			if constexpr(IS_SINGLE_SPLIT_AXIS)
			{
				const auto singleSplitAxis = node.getSplitAxis(0);
				PH_ASSERT_LT(singleSplitAxis, isNegDir.size());
				ci = isNegDir[singleSplitAxis] ? N - 1 - i : i;
				//ci = singleSplitAxisOrderTable[isNegDir[singleSplitAxis]][i];
			}

			real minT = hitTs[ci];
			if constexpr(sseCtx.isSupported())
			{
				//isChildHit = (sseHitMask >> ci) & 0b1;
				minT = sseHitTs[ci];
			}

			if(minT < longestSegment.getMaxT())
			{
				if(node.isLeaf(ci))
				{
					const auto numItems = node.numItems(ci);
					for(std::size_t ii = 0; ii < numItems; ++ii)
					{
						const Item& item = m_items[node.getItemOffset(ci) + ii];

						const auto optHitT = intersectionTester(item, longestSegment);
						if(optHitT)
						{
							longestSegment.setMaxT(*optHitT);
							hasHit = true;
						}
					}
				}
				else
				{
					PH_ASSERT_LE(node.getChildOffset(ci), std::numeric_limits<Index>::max());
					nextChildNodes[numNextChildNodes] = static_cast<Index>(node.getChildOffset(ci));
					++numNextChildNodes;
				}
			}
		}

		// Push nodes to stack such that the nearest one is on top
		while(numNextChildNodes > 0)
		{
			--numNextChildNodes;
			todoNodes.push(nextChildNodes[numNextChildNodes]);
		}

		if(todoNodes.isEmpty())
		{
			break;
		}
		else
		{
			currentNodeIndex = todoNodes.top();
			todoNodes.pop();
		}
	}
	
	return hasHit;
}

template<std::size_t N, typename Item, typename Index>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::isEmpty() const
{
	return m_numNodes == 0;
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::getRoot() const
-> const NodeType&
{
	PH_ASSERT(m_nodes);
	PH_ASSERT_GT(m_numNodes, 0);
	return m_nodes[0];
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::numNodes() const
-> std::size_t
{
	return m_numNodes;
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::numItems() const
-> std::size_t
{
	return m_numItems;
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::memoryUsage() const
-> std::size_t
{
	return sizeof(*this) + m_numNodes * sizeof(NodeType) + m_numItems * sizeof(Item);
}

template<std::size_t N, typename Item, typename Index>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::convertChildNodesRecursive(
	const TBvhInfoNode<N, Item>* const infoNode)
{
	PH_ASSERT(infoNode);

	// Leaf node should be converted already
	PH_ASSERT_MSG(!infoNode->isLeaf(), "this method expects internal node as input");

	m_isSingleSplitAxisNodes &= infoNode->isSingleSplitAxis();

	const auto nodeIndex = m_numNodes;
	NodeType* const node = &(m_nodes[nodeIndex]);
	m_numNodes += 1;

	for(std::size_t ci = 0; ci < infoNode->numChildren(); ++ci)
	{
		const TBvhInfoNode<N, Item>* const childInfoNode = infoNode->getChild(ci);
		const auto splitAxis = infoNode->getSplitAxis(ci);
		if(!childInfoNode)
		{
			node->setEmptyLeaf(ci, splitAxis);
			continue;
		}
		
		if(childInfoNode->isLeaf())
		{
			const auto itemOffset = m_numItems;
			for(std::size_t i = 0; i < childInfoNode->getItems().size(); ++i)
			{
				m_items[itemOffset + i] = childInfoNode->getItems()[i].item;
			}
			m_numItems += static_cast<Index>(childInfoNode->getItems().size());

			node->setLeaf(
				ci,
				childInfoNode->getAABB(),
				itemOffset,
				splitAxis,
				childInfoNode->getItems().size());
		}
		else if(childInfoNode->isInternal())
		{
			const auto childNodeIndex = m_numNodes;
			convertChildNodesRecursive(childInfoNode);

			node->setInternal(
				ci,
				childInfoNode->getAABB(),
				childNodeIndex,
				splitAxis);
		}
		else
		{
			PH_ASSERT_UNREACHABLE_SECTION();
		}
	}
}

template<std::size_t N, typename Item, typename Index>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::refitBuffer(const std::size_t nodeBufferSize, const std::size_t itemBufferSize)
{
	// For potentially better performance on buffer content copying
	static_assert(std::is_trivially_copy_assignable_v<NodeType>);

	PH_ASSERT_LE(m_numNodes, nodeBufferSize);
	PH_ASSERT_LE(m_numItems, itemBufferSize);

	if(m_numNodes != nodeBufferSize)
	{
		auto nodes = std::make_unique<NodeType[]>(m_numNodes);
		std::copy_n(m_nodes.get(), m_numNodes, nodes.get());

		m_nodes = std::move(nodes);
	}

	if(m_numItems != itemBufferSize)
	{
		auto items = std::make_unique<Item[]>(m_numItems);
		std::copy_n(m_items.get(), m_numItems, items.get());

		m_items = std::move(items);
	}
}

template<std::size_t N, typename Item, typename Index>
inline constexpr auto TLinearDepthFirstWideBvh<N, Item, Index>
::makeSingleSplitAxisOrderTable()
-> std::array<std::array<std::size_t, N>, 2>
{
	std::array<std::array<std::size_t, N>, 2> table;
	for(std::size_t i = 0; i < N; ++i)
	{
		// Positive segment direction
		table[0][i] = i;

		// Negative segment direction
		table[1][i] = N - 1 - i;
	}
	return table;
}

}// end namespace ph::math
