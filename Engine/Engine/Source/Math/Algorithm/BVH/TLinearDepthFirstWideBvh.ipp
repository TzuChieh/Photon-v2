#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TBvhItemInfo.h"
#include "Math/Algorithm/BVH/TBvhSseF32Context.h"
#include "Math/Algorithm/traversal_concepts.h"
#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

#include <type_traits>
#include <array>
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
			m_numItems += rootNode->getItems().size();

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
	// Traversal states
	TArrayStack<Index, TRAVERSAL_STACK_SIZE> todoNodes;
	Index currentNodeIndex = 0;
	TLineSegment<real> longestSegment(segment);
	bool hasHit = false;

	// Precompute common values

	const auto rcpSegmentDir = segment.getDir().rcp();

	std::array<bool, 3> isNegDir;
	if constexpr(IS_SINGLE_SPLIT_AXIS)
	{
		isNegDir = {
			segment.getDir().x() < 0,
			segment.getDir().y() < 0,
			segment.getDir().z() < 0};
	}

	TBvhSseF32Context<N, Index> ctx;
	ctx.setSegment(segment.getOrigin(), rcpSegmentDir);

	// Traverse nodes
	while(true)
	{
		PH_ASSERT_LT(currentNodeIndex, m_numNodes);
		const NodeType& node = m_nodes[currentNodeIndex];

		ctx.setNode(node);
		const auto hitMask = ctx.intersectAabbVolumes(longestSegment.getMinT(), longestSegment.getMaxT());

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
			}

			/*const auto [aabbMinT, aabbMaxT] = node.getAABB(ci).isIntersectingVolume<IS_ROBUST>(
				longestSegment, rcpSegmentDir);*/
			//if(aabbMinT <= aabbMaxT)
			if((hitMask >> ci) & 0b1)
			{
				if(node.isLeaf(ci))
				{
					const auto numItems = node.numItems(ci);
					for(std::size_t i = 0; i < numItems; ++i)
					{
						const Item& item = m_items[node.getItemOffset(ci) + i];

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

template<std::size_t N, typename Item, typename IndexType>
inline bool TLinearDepthFirstWideBvh<N, Item, IndexType>
::isEmpty() const
{
	return m_numNodes == 0;
}

template<std::size_t N, typename Item, typename IndexType>
inline auto TLinearDepthFirstWideBvh<N, Item, IndexType>
::getRoot() const
-> const NodeType&
{
	PH_ASSERT(m_nodes);
	PH_ASSERT_GT(m_numNodes, 0);
	return m_nodes[0];
}

template<std::size_t N, typename Item, typename IndexType>
inline auto TLinearDepthFirstWideBvh<N, Item, IndexType>
::numNodes() const
-> std::size_t
{
	return m_numNodes;
}

template<std::size_t N, typename Item, typename IndexType>
inline auto TLinearDepthFirstWideBvh<N, Item, IndexType>
::numItems() const
-> std::size_t
{
	return m_numItems;
}

template<std::size_t N, typename Item, typename IndexType>
inline auto TLinearDepthFirstWideBvh<N, Item, IndexType>
::memoryUsage() const
-> std::size_t
{
	return sizeof(*this) + m_numNodes * sizeof(NodeType) + m_numItems * sizeof(Item);
}

template<std::size_t N, typename Item, typename IndexType>
inline void TLinearDepthFirstWideBvh<N, Item, IndexType>
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
			m_numItems += childInfoNode->getItems().size();

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

template<std::size_t N, typename Item, typename IndexType>
inline void TLinearDepthFirstWideBvh<N, Item, IndexType>
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

}// end namespace ph::math
