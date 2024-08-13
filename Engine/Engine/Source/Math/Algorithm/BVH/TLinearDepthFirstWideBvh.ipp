#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TBvhItemInfo.h"
#include "Math/Algorithm/traversal_concepts.h"
#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

#include <type_traits>

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
	m_nodes = std::make_unique<TWideBvhNode<N, Item, Index>[]>(totalInfoNodes);
	m_items = std::make_unique<Item[]>(totalItems);

	// Can directly convert if the branch factor matches
	if constexpr(SrcN == N)
	{
		if(rootNode->isLeaf())
		{
			const auto itemOffset = m_numItems;
			for(std::size_t i = 0; i < rootNode->getItems().size(); ++i)
			{
				m_items[itemOffset + i] = rootNode->getItems()[i].item;
			}
			m_numItems += rootNode->getItems().size();

			m_numNodes = 1;
			m_nodes[0].setLeaf(
				0,
				rootNode->getAABB(),
				itemOffset,
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
template<typename TesterFunc, bool IS_ROBUST>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const
{
	static_assert(CItemSegmentIntersectionTester<TesterFunc, Item>);

	if(isEmpty())
	{
		return false;
	}

	PH_ASSERT(nodes);
	PH_ASSERT(items);

	// Traversal states
	TArrayStack<Index, TRAVERSAL_STACK_SIZE> todoNodes;
	Index currentNodeIndex = 0;
	TLineSegment<real> longestSegment(segment);
	bool hasHit = false;

	// Precompute common values
	const bool isNegDir[3] = {
		segment.getDir().x() < 0,
		segment.getDir().y() < 0,
		segment.getDir().z() < 0};
	const auto rcpSegmentDir = segment.getDir().rcp();

	// Traverse nodes
	while(true)
	{
		PH_ASSERT_LT(currentNodeIndex, numNodes);
		const NodeType& node = nodes[currentNodeIndex];

		const auto [aabbMinT, aabbMaxT] = node.getAABB().isIntersectingVolume<IS_ROBUST>(
			longestSegment, rcpSegmentDir);
		if(aabbMinT <= aabbMaxT)
		{
			if(node.isLeaf())
			{
				for(std::size_t i = 0; i < node.numItems(); ++i)
				{
					const Item& item = items[node.getItemOffset() + i];

					const auto optHitT = intersectionTester(item, longestSegment);
					if(optHitT)
					{
						longestSegment.setMaxT(*optHitT);
						hasHit = true;
					}
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
			else
			{
				if(isNegDir[node.getSplitAxis()])
				{
					todoNodes.push(currentNodeIndex + 1);
					currentNodeIndex = node.getChildOffset();
				}
				else
				{
					todoNodes.push(node.getChildOffset());
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else
		{
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
	}
	
	return hasHit;
}

template<std::size_t N, typename Item, typename IndexType>
inline bool TLinearDepthFirstWideBvh<N, Item, IndexType>
::isEmpty() const
{
	return numNodes == 0;
}

template<std::size_t N, typename Item, typename IndexType>
inline auto TLinearDepthFirstWideBvh<N, Item, IndexType>
::getRoot() const
-> const NodeType&
{
	PH_ASSERT(nodes);
	PH_ASSERT_GT(numNodes, 0);
	return nodes[0];
}

template<std::size_t N, typename Item, typename IndexType>
inline void TLinearDepthFirstWideBvh<N, Item, IndexType>
::convertChildNodesRecursive(
	const TBvhInfoNode<N, Item>* const infoNode)
{
	PH_ASSERT(infoNode);

	// Leaf node should be converted already
	PH_ASSERT_MSG(!infoNode->isLeaf(), "this method expects internal node as input");

	const auto nodeIndex = m_numNodes;
	NodeType* const node = &(m_nodes[nodeIndex]);
	m_numNodes += 1;

	for(std::size_t ci = 0; ci < infoNode->numChildren(); ++ci)
	{
		const TBvhInfoNode<N, Item>* const childInfoNode = infoNode->getChild(ci);
		if(!childInfoNode)
		{
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
				childInfoNode->getItems().size());
		}
		else if(childInfoNode->isInternal())
		{
			const auto childNodeIndex = m_numNodes;
			convertNodesRecursive(childInfoNode);

			node->setInternal(
				ci,
				childInfoNode->getAABB(),
				childNodeIndex,
				childInfoNode->getSplitAxis());
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
