#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.h"
#include "Math/Algorithm/traversal_concepts.h"
#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

#include <cstddef>

namespace ph::math
{

template<typename Item, typename Index>
template<typename TesterFunc, bool IS_ROBUST>
inline bool TLinearDepthFirstBinaryBvh<Item, Index>
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
					currentNodeIndex = node.getSecondChildOffset();
				}
				else
				{
					todoNodes.push(node.getSecondChildOffset());
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

template<typename Item, typename IndexType>
inline bool TLinearDepthFirstBinaryBvh<Item, IndexType>
::isEmpty() const
{
	return numNodes == 0;
}

template<typename Item, typename IndexType>
inline auto TLinearDepthFirstBinaryBvh<Item, IndexType>
::getRoot() const
-> const NodeType&
{
	PH_ASSERT(nodes);
	PH_ASSERT_GT(numNodes, 0);
	return nodes[0];
}

}// end namespace ph::math
