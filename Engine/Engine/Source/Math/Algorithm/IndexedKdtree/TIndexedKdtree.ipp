#pragma once

#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.h"

#include <limits>
#include <array>
#include <cmath>
#include <algorithm>

namespace ph::math
{

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index>
inline TIndexedKdtree<IndexToItem, ItemToAABB, Index>::
TIndexedKdtree(
	const std::size_t   numItems,
	IndexToItem         indexToItem,
	ItemToAABB          itemToAABB,
	IndexedKdtreeParams params) :

	m_numItems   (numItems),
	m_indexToItem(std::move(indexToItem)),
	m_rootAABB   (),
	m_nodeBuffer (),
	m_numNodes   (0),
	m_itemIndices()
{
	if(numItems > 0)
	{
		build(std::move(itemToAABB), params);
	}
}

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index>
template<
	typename TesterFunc>
inline auto TIndexedKdtree<IndexToItem, ItemToAABB, Index>::
nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const
-> bool
{
	static_assert(CItemSegmentIntersectionTester<TesterFunc, Item>);

	PH_ASSERT_GT(m_numNodes, 0);

	struct NodeState
	{
		const Node* node;
		real        minT;
		real        maxT;
	};

	constexpr bool TEST_WITH_ITEM_IDX = CItemSegmentIntersectionTesterWithIndex<TesterFunc, Item>;
	constexpr int MAX_STACK_HEIGHT = 64;

	real minT, maxT;
	if(!m_rootAABB.isIntersectingVolume(segment, &minT, &maxT))
	{
		return false;
	}
	TLineSegment<real> intersectSegment(segment.getOrigin(), segment.getDirection(), minT, maxT);

	const Vector3R rcpRayDir(segment.getDirection().rcp());

	std::array<NodeState, MAX_STACK_HEIGHT> nodeStack;
	int stackHeight = 0;
	bool hasHit = false;
	const Node* currentNode = &(m_nodeBuffer[0]);
	while(true)
	{
		if(!currentNode->isLeaf())
		{
			const int  splitAxis   = currentNode->splitAxisIndex();
			const real splitPlaneT = (currentNode->splitPos() - segment.getOrigin()[splitAxis]) * rcpRayDir[splitAxis];

			const Node* nearHitNode;
			const Node* farHitNode;
			if(
				(segment.getOrigin()[splitAxis] < currentNode->splitPos()) ||
				(segment.getOrigin()[splitAxis] == currentNode->splitPos() && 
				 segment.getDirection()[splitAxis] <= 0))
			{
				nearHitNode = currentNode + 1;
				farHitNode  = &(m_nodeBuffer[currentNode->positiveChildIndex()]);
			}
			else
			{
				nearHitNode = &(m_nodeBuffer[currentNode->positiveChildIndex()]);
				farHitNode  = currentNode + 1;
			}

			// Case I: Only near node is hit.
			//         (split plane is beyond t-max or behind ray origin)
			if(splitPlaneT > maxT || splitPlaneT < 0)
			{
				currentNode = nearHitNode;
			}
			// Case II: Only far node is hit.
			//          (split plane is between ray origin and t-min)
			else if(splitPlaneT < minT)
			{
				currentNode = farHitNode;
			}
			// Case III: Both near and far nodes are hit.
			//           (split plane is within [t-min, t-max])
			else
			{
				PH_ASSERT_LT(stackHeight, MAX_STACK_HEIGHT);

				nodeStack[stackHeight].node = farHitNode;
				nodeStack[stackHeight].minT = splitPlaneT;
				nodeStack[stackHeight].maxT = maxT;
				++stackHeight;

				currentNode = nearHitNode;
				maxT        = splitPlaneT;
			}
		}
		// current node is leaf
		else
		{
			const std::size_t numItems = currentNode->numItems();

			if(numItems == 1)
			{
				const auto itemIndex = currentNode->singleItemDirectIndex();
				const Item& item = m_indexToItem(itemIndex);

				std::optional<real> hitT;
				if constexpr(TEST_WITH_ITEM_IDX)
				{
					hitT = intersectionTester(item, intersectSegment, itemIndex);
				}
				else
				{
					hitT = intersectionTester(item, intersectSegment);
				}

				if(hitT)
				{
					intersectSegment.setMaxT(*hitT);
					hasHit = true;
				}
			}
			else
			{
				for(std::size_t i = 0; i < numItems; ++i)
				{
					const Index itemIndex = m_itemIndices[currentNode->indexBufferOffset() + i];
					const Item& item = m_indexToItem(itemIndex);

					std::optional<real> hitT;
					if constexpr(TEST_WITH_ITEM_IDX)
					{
						hitT = intersectionTester(item, intersectSegment, itemIndex);
					}
					else
					{
						hitT = intersectionTester(item, intersectSegment);
					}

					if(hitT)
					{
						intersectSegment.setMaxT(*hitT);
						hasHit = true;
					}
				}
			}

			if(stackHeight > 0)
			{
				--stackHeight;
				currentNode = nodeStack[stackHeight].node;
				minT        = nodeStack[stackHeight].minT;
				maxT        = nodeStack[stackHeight].maxT;

				// Early-out if the test segment cannot reach the next node
				if(intersectSegment.getMaxT() < minT)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}// end is leaf node
	}// end infinite loop

	return hasHit;
}

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index>
inline auto TIndexedKdtree<IndexToItem, ItemToAABB, Index>::
getAABB() const
-> AABB3D
{
	PH_ASSERT(!isEmpty());

	return m_rootAABB;
}

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index>
inline auto TIndexedKdtree<IndexToItem, ItemToAABB, Index>::

isEmpty() const
-> bool
{
	return m_numItems == 0;
}

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index>
inline void TIndexedKdtree<IndexToItem, ItemToAABB, Index>::
build(ItemToAABB itemToAABB, IndexedKdtreeParams params)
{
	PH_ASSERT_GT(m_numItems, 0);

	const auto maxNodeDepth = static_cast<std::size_t>(8 + 1.3 * std::log2(m_numItems) + 0.5);

	// Cache item AABB and calculate root AABB
	std::vector<AABB3D> itemAABBs;
	m_rootAABB = itemToAABB(m_indexToItem(0));
	for(std::size_t i = 0; i < m_numItems; ++i)
	{
		const AABB3D aabb = itemToAABB(m_indexToItem(i));

		itemAABBs.push_back(aabb);
		m_rootAABB.unionWith(aabb);
	}

	std::unique_ptr<Index[]> negativeItemIndicesCache(new Index[m_numItems]);
	std::unique_ptr<Index[]> positiveItemIndicesCache(new Index[m_numItems * maxNodeDepth]);

	PH_ASSERT(m_numItems - 1 <= std::numeric_limits<Index>::max());
	for(std::size_t i = 0; i < m_numItems; ++i)
	{
		negativeItemIndicesCache[i] = static_cast<Index>(i);
	}

	std::array<std::unique_ptr<ItemEndpoint[]>, 3> endPointsCache;
	for(auto&& cache : endPointsCache)
	{
		cache = std::unique_ptr<ItemEndpoint[]>(new ItemEndpoint[m_numItems * 2]);
	}

	buildNodeRecursive(
		0, 
		m_rootAABB, 
		negativeItemIndicesCache.get(),
		m_numItems,
		0,
		0,
		itemAABBs,
		maxNodeDepth,
		params,
		negativeItemIndicesCache.get(),
		positiveItemIndicesCache.get(),
		endPointsCache);
}

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index>
inline void TIndexedKdtree<IndexToItem, ItemToAABB, Index>::
buildNodeRecursive(
	std::size_t nodeIndex,
	const AABB3D& nodeAABB,
	const Index* nodeItemIndices,
	std::size_t numNodeItems,
	std::size_t currentNodeDepth,
	std::size_t currentBadRefines,
	const std::vector<AABB3D>& itemAABBs,
	const std::size_t maxNodeDepth,
	IndexedKdtreeParams params,
	Index* negativeItemIndicesCache,
	Index* positiveItemIndicesCache,
	std::array<std::unique_ptr<ItemEndpoint[]>, 3>& endpointsCache)
{
	++m_numNodes;
	if(m_numNodes > m_nodeBuffer.size())
	{
		m_nodeBuffer.resize(m_numNodes * 2);
	}
	PH_ASSERT(nodeIndex < m_nodeBuffer.size());

	if(currentNodeDepth == maxNodeDepth || numNodeItems <= params.getMaxNodeItems())
	{
		m_nodeBuffer[nodeIndex] = Node::makeLeaf(nodeItemIndices, numNodeItems, m_itemIndices);
		return;
	}

	const real     noSplitCost        = params.getInteractCost() * static_cast<real>(numNodeItems);
	const real     rcpNodeSurfaceArea = 1.0_r / nodeAABB.getSurfaceArea();
	const Vector3R nodeExtents        = nodeAABB.getExtents();

	real        bestSplitCost     = std::numeric_limits<real>::max();
	int         bestAxis          = -1;
	std::size_t bestEndpointIndex = std::numeric_limits<std::size_t>::max();
	int         axis              = static_cast<int>(nodeExtents.maxDimension());
	int         numSplitTrials    = 0;
	while(bestAxis == -1 && numSplitTrials < 3)
	{
		for(std::size_t i = 0; i < numNodeItems; ++i)
		{
			const Index   itemIndex = nodeItemIndices[i];
			const AABB3D& itemAABB  = itemAABBs[itemIndex];
			endpointsCache[axis][2 * i]     = ItemEndpoint{itemAABB.getMinVertex()[axis], itemIndex, EEndpoint::MIN};
			endpointsCache[axis][2 * i + 1] = ItemEndpoint{itemAABB.getMaxVertex()[axis], itemIndex, EEndpoint::MAX};
		}

		std::sort(&(endpointsCache[axis][0]), &(endpointsCache[axis][2 * numNodeItems]), 
			[](const ItemEndpoint& a, const ItemEndpoint& b) -> bool
			{
				return a.position != b.position ? a.position < b.position 
												: a.type     < b.type;
			});

		std::size_t numNegativeItems = 0;
		std::size_t numPositiveItems = numNodeItems;
		for(std::size_t e = 0; e < 2 * numNodeItems; ++e)
		{
			if(endpointsCache[axis][e].type == EEndpoint::MAX)
			{
				--numPositiveItems;
			}

			// check if the split point is a reasonable one (within node AABB)
			real endpoint = endpointsCache[axis][e].position;
			if(endpoint > nodeAABB.getMinVertex()[axis] &&
			   endpoint < nodeAABB.getMaxVertex()[axis])
			{
				Vector3R endpointMinVertex = nodeAABB.getMinVertex();
				Vector3R endpointMaxVertex = nodeAABB.getMaxVertex();
				endpointMinVertex[axis] = endpoint;
				endpointMaxVertex[axis] = endpoint;

				const real probNegative     = AABB3D(nodeAABB.getMinVertex(), endpointMaxVertex).getSurfaceArea() * rcpNodeSurfaceArea;
				const real probPositive     = AABB3D(endpointMinVertex, nodeAABB.getMaxVertex()).getSurfaceArea() * rcpNodeSurfaceArea;
				const real emptyBonus       = (numNegativeItems == 0 || numPositiveItems == 0) ? params.getEmptyBonus() : 0.0_r;
				const real currentSplitCost = params.getTraversalCost() + (1.0_r - emptyBonus) * params.getInteractCost() *
					(probNegative * static_cast<real>(numNegativeItems) + probPositive * static_cast<real>(numPositiveItems));

				if(currentSplitCost < bestSplitCost)
				{
					bestSplitCost     = currentSplitCost;
					bestAxis          = axis;
					bestEndpointIndex = e;
				}
			}

			if(endpointsCache[axis][e].type == EEndpoint::MIN)
			{
				++numNegativeItems;
			}
		}

		++numSplitTrials;
		axis = (axis + 1) % 3;
	}
	
	std::size_t newNumBadRefines = currentBadRefines;
	if(bestSplitCost > noSplitCost)
	{
		++newNumBadRefines;
	}

	if((bestSplitCost > 4 * noSplitCost && numNodeItems < 16) || 
	   bestAxis == -1 ||
	   newNumBadRefines == 3)
	{
		m_nodeBuffer[nodeIndex] = Node::makeLeaf(nodeItemIndices, numNodeItems, m_itemIndices);
		return;
	}

	std::size_t numNegativeItems = 0;
	for(std::size_t e = 0; e < bestEndpointIndex; ++e)
	{
		if(endpointsCache[bestAxis][e].type == EEndpoint::MIN)
		{
			negativeItemIndicesCache[numNegativeItems++] = endpointsCache[bestAxis][e].index;
		}
	}
	std::size_t numPositiveItems = 0;
	for(std::size_t e = bestEndpointIndex + 1; e < 2 * numNodeItems; ++e)
	{
		if(endpointsCache[bestAxis][e].type == EEndpoint::MAX)
		{
			positiveItemIndicesCache[numPositiveItems++] = endpointsCache[bestAxis][e].index;
		}
	}
	PH_ASSERT(numNegativeItems <= numNodeItems && numPositiveItems <= numNodeItems);

	const real bestSplitPos = endpointsCache[bestAxis][bestEndpointIndex].position;

	Vector3R splitPosMinVertex = nodeAABB.getMinVertex();
	Vector3R splitPosMaxVertex = nodeAABB.getMaxVertex();
	splitPosMinVertex[bestAxis] = bestSplitPos;
	splitPosMaxVertex[bestAxis] = bestSplitPos;
	const AABB3D negativeNodeAABB(nodeAABB.getMinVertex(), splitPosMaxVertex);
	const AABB3D positiveNodeAABB(splitPosMinVertex, nodeAABB.getMaxVertex());
	
	buildNodeRecursive(
		nodeIndex + 1, 
		negativeNodeAABB, 
		negativeItemIndicesCache,
		numNegativeItems,
		currentNodeDepth + 1,
		newNumBadRefines,
		itemAABBs,
		maxNodeDepth,
		params,
		negativeItemIndicesCache,
		positiveItemIndicesCache + numNodeItems,
		endpointsCache);

	const std::size_t positiveChildIndex = m_numNodes;
	m_nodeBuffer[nodeIndex] = Node::makeInner(bestSplitPos, bestAxis, positiveChildIndex);

	buildNodeRecursive(
		positiveChildIndex,
		positiveNodeAABB,
		positiveItemIndicesCache,
		numPositiveItems,
		currentNodeDepth + 1,
		newNumBadRefines,
		itemAABBs,
		maxNodeDepth,
		params,
		negativeItemIndicesCache,
		positiveItemIndicesCache + numNodeItems,
		endpointsCache);
}

}// end namespace ph::math
