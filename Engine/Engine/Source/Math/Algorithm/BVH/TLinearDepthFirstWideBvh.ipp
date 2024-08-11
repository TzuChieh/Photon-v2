#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TBvhItemInfo.h"
#include "Math/Algorithm/traversal_concepts.h"
#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

namespace ph::math
{

template<std::size_t N, typename Item, typename Index>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::build(
	const TBvhInfoNode<Item>* const rootNode,
	const std::size_t totalInfoNodes,
	const std::size_t totalItems)
{
	PH_ASSERT(rootNode);

	// Allocate memory for nodes and items
	m_nodes = std::make_unique<TBinaryBvhNode<Item, Index>[]>(totalInfoNodes);
	m_items = std::make_unique<Item[]>(totalItems);

	// Flatten the info tree into a more compact representation
	buildLinearDepthFirstBinaryBvhRecursive(rootNode, out_bvh);

	PH_ASSERT_EQ(m_numNodes, totalInfoNodes);
	PH_ASSERT_EQ(m_numItems, totalItems);
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
::buildNodeRecursive(
	const TBvhInfoNode<Item>* infoNode)
{
	const auto nodeIndex = m_numNodes;
	const auto itemOffset = m_numItems;

	if(infoNode->isBinaryLeaf())
	{
		for(std::size_t i = 0; i < infoNode->items.size(); ++i)
		{
			m_items[itemOffset + i] = infoNode->items[i].item;
		}
		m_numItems += infoNode->items.size();

		m_nodes[nodeIndex] = NodeType::makeLeaf(
			infoNode->aabb,
			itemOffset,
			infoNode->items.size());
		m_numNodes += 1;
	}
	else if(infoNode->isBinaryInternal())
	{
		NodeType* node = &(m_nodes[nodeIndex]);
		m_numNodes += 1;

		buildLinearDepthFirstBinaryBvhRecursive(infoNode->children[0]);

		const auto secondChildOffset = m_numNodes;
		buildLinearDepthFirstBinaryBvhRecursive(infoNode->children[1]);

		*node = NodeType::makeInternal(
			infoNode->aabb,
			secondChildOffset,
			infoNode->splitAxis);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph::math
