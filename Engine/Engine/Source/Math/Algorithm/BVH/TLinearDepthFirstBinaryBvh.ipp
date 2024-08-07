#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.h"
#include "Math/Algorithm/traversal_concepts.h"
#include "Utility/TArrayStack.h"

#include <Common/assertion.h>

namespace ph::math
{

template<typename Item, typename IndexType>
template<typename TesterFunc>
inline bool TLinearDepthFirstBinaryBvh<Item, IndexType>
::nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const
{
	static_assert(CItemSegmentIntersectionTester<TesterFunc, Item>);

	if(isEmpty())
	{
		return false;
	}

	PH_ASSERT(nodes);
	PH_ASSERT(items);

	const bool isNegDir[3] = {
		segment.getDir().x() < 0,
		segment.getDir().y() < 0,
		segment.getDir().z() < 0};

	TArrayStack<IndexType, TRAVERSAL_STACK_SIZE> todoNodes;
	IndexType currentNodeIndex = 0;

	TLineSegment<real> bvhSegment(segment);
	bool hasHit = false;

	// TODO: possibly make use of minT & maxT found by AABB intersection?

	while(true)
	{
		PH_ASSERT_LT(currentNodeIndex, numNodes);
		const NodeType& node = nodes[currentNodeIndex];

		if(node.aabb.isIntersectingVolume(bvhSegment))
		{
			if(node.isLeaf())
			{
				for(IndexType i = 0; i < node.numItems; ++i)
				{
					const Item& item = items[node.itemOffset + i];

					const auto optHitT = intersectionTester(item, bvhSegment);
					if(optHitT)
					{
						bvhSegment.setMaxT(*optHitT);
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
				if(isNegDir[node.splitAxis])
				{
					todoNodes.push(currentNodeIndex + 1);
					currentNodeIndex = node.secondChildOffset;
				}
				else
				{
					todoNodes.push(node.secondChildOffset);
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
