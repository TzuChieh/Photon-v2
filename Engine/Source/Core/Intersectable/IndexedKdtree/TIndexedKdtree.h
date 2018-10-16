#pragma once

#include "Core/Bound/AABB3D.h"
#include "Core/Intersectable/IndexedKdtree/IndexedKdtreeNode.h"
#include "Utility/utility.h"
#include "Core/Intersectable/IndexedKdtree/IndexedItemEndpoint.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"

#include <vector>
#include <utility>
#include <memory>
#include <array>
#include <cmath>
#include <limits>
#include <algorithm>

namespace ph
{

template<typename Item, typename Index>
class TIndexedKdtree
{
public:
	TIndexedKdtree(
		int traversalCost, 
		int intersectionCost,
		float emptyBonus,
		std::size_t maxNodeItems);

	void build(std::vector<Item>&& items);
	bool isIntersecting(const Ray& ray, HitProbe& probe) const;
	void getAABB(AABB3D* out_aabb) const;

private:
	std::vector<Item> m_items;
	int m_traversalCost;
	int m_intersectionCost;
	float m_emptyBonus;
	std::size_t m_maxNodeItems;
	std::size_t m_maxNodeDepth;
	AABB3D m_rootAABB;
	std::vector<IndexedKdtreeNode> m_nodeBuffer;
	std::size_t m_numNodes;
	std::vector<Index> m_itemIndices;

	void buildNodeRecursive(
		std::size_t nodeIndex,
		const AABB3D& nodeAABB,
		const Index* nodeItemIndices,
		std::size_t numNodeItems,
		std::size_t currentNodeDepth,
		std::size_t currentBadRefines,
		const std::vector<AABB3D>& itemAABBs,
		Index* negativeItemIndicesCache,
		Index* positiveItemIndicesCache,
		std::array<std::unique_ptr<IndexedItemEndpoint[]>, 3>& endpointsCache);
};

// In-header Implementations:

template<typename Item, typename Index>
inline TIndexedKdtree<Item, Index>::TIndexedKdtree(

	const int traversalCost,
	const int intersectionCost,
	const float emptyBonus,
	const std::size_t maxNodeItems) :

	m_items(),
	m_traversalCost(traversalCost),
	m_intersectionCost(intersectionCost),
	m_emptyBonus(emptyBonus),
	m_maxNodeItems(maxNodeItems),
	m_maxNodeDepth(0),
	m_rootAABB(),
	m_nodeBuffer(),
	m_numNodes(0),
	m_itemIndices()
{}

template<typename Item, typename Index>
inline void TIndexedKdtree<Item, Index>::build(std::vector<Item>&& items)
{
	m_items = std::move(items);
	if(m_items.empty())
	{
		return;
	}

	m_maxNodeDepth = static_cast<std::size_t>(8 + 1.3 * std::log2(m_items.size()) + 0.5);

	std::vector<AABB3D> itemAABBs;
	regular_access(m_items.front()).calcAABB(&m_rootAABB);
	for(const auto& item : m_items)
	{
		AABB3D aabb;
		regular_access(item).calcAABB(&aabb);

		itemAABBs.push_back(aabb);
		m_rootAABB.unionWith(aabb);
	}

	std::unique_ptr<Index[]> negativeItemIndicesCache(new Index[m_items.size()]);
	std::unique_ptr<Index[]> positiveItemIndicesCache(new Index[m_items.size() * m_maxNodeDepth]);
	for(std::size_t i = 0; i < m_items.size(); ++i)
	{
		negativeItemIndicesCache[i] = i;
	}

	std::array<std::unique_ptr<IndexedItemEndpoint[]>, 3> endPointsCache;
	for(auto&& cache : endPointsCache)
	{
		cache = std::unique_ptr<IndexedItemEndpoint[]>(new IndexedItemEndpoint[m_items.size() * 2]);
	}

	buildNodeRecursive(
		0, 
		m_rootAABB, 
		negativeItemIndicesCache.get(),
		m_items.size(),
		0,
		0,
		itemAABBs,
		negativeItemIndicesCache.get(),
		positiveItemIndicesCache.get(),
		endPointsCache);
}

template<typename Item, typename Index>
inline bool TIndexedKdtree<Item, Index>::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	struct NodeState
	{
		const IndexedKdtreeNode* node;
		real minT;
		real maxT;
	};

	real minT, maxT;
	if(!m_rootAABB.isIntersectingVolume(ray, &minT, &maxT))
	{
		return false;
	}

	const Vector3R reciRayDir(ray.getDirection().reciprocal());

	std::array<NodeState, 64> nodeStack;
	int stackHeight = 0;
	const IndexedKdtreeNode* currentNode = &(m_nodeBuffer[0]);
	while(true)
	{
		if(!currentNode->isLeaf())
		{
			const int  splitAxis   = currentNode->splitAxisIndex();
			const real splitPlaneT = (currentNode->getSplitPos() - ray.getOrigin()[splitAxis]) * reciRayDir[splitAxis];

			const IndexedKdtreeNode* nearHitNode;
			const IndexedKdtreeNode* farHitNode;
			if((ray.getOrigin()[splitAxis] < currentNode->getSplitPos()) ||
			   (ray.getOrigin()[splitAxis] == currentNode->getSplitPos() && ray.getDirection()[splitAxis] <= 0))
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
			Ray segment(ray.getOrigin(), ray.getDirection(), minT, maxT);

			if(numItems == 1)
			{
				const Item& item = m_items[currentNode->getSingleItemDirectIndex()];

				HitProbe hitProbe(probe);
				if(regular_access(item).isIntersecting(segment, hitProbe))
				{
					probe = hitProbe;
					return true;
				}
			}
			else
			{
				HitProbe closestProbe;
				for(std::size_t i = 0; i < numItems; ++i)
				{
					const Index itemIndex = m_itemIndices[currentNode->getItemIndexOffset() + i];
					const Item& item      = m_items[itemIndex];

					HitProbe hitProbe(probe);
					if(regular_access(item).isIntersecting(segment, hitProbe))
					{
						if(hitProbe.getHitRayT() < closestProbe.getHitRayT())
						{
							closestProbe = hitProbe;
							segment.setMaxT(hitProbe.getHitRayT());
						}
					}
				}

				if(closestProbe.getHitRayT() < std::numeric_limits<real>::max())
				{
					probe = closestProbe;
					return true;
				}
			}

			if(stackHeight > 0)
			{
				--stackHeight;
				currentNode = nodeStack[stackHeight].node;
				minT        = nodeStack[stackHeight].minT;
				maxT        = nodeStack[stackHeight].maxT;
			}
			else
			{
				break;
			}
		}// end is leaf node
	}// end infinite loop

	return false;
}

template<typename Item, typename Index>
inline void TIndexedKdtree<Item, Index>::buildNodeRecursive(
	const std::size_t nodeIndex,
	const AABB3D& nodeAABB,
	const Index* const nodeItemIndices,
	const std::size_t numNodeItems,
	const std::size_t currentNodeDepth,
	const std::size_t currentBadRefines,
	const std::vector<AABB3D>& itemAABBs,
	Index* const negativeItemIndicesCache,
	Index* const positiveItemIndicesCache,
	std::array<std::unique_ptr<IndexedItemEndpoint[]>, 3>& endpointsCache)
{
	++m_numNodes;
	if(m_numNodes > m_nodeBuffer.size())
	{
		m_nodeBuffer.resize(m_numNodes * 2);
	}

	if(currentNodeDepth == m_maxNodeDepth || numNodeItems <= m_maxNodeItems)
	{
		m_nodeBuffer[nodeIndex] = IndexedKdtreeNode::makeLeaf(nodeItemIndices, numNodeItems, m_itemIndices);
		return;
	}

	const real     noSplitCost         = m_intersectionCost * static_cast<real>(numNodeItems);
	const real     reciNodeSurfaceArea = 1.0_r / nodeAABB.calcSurfaceArea();
	const Vector3R nodeExtents         = nodeAABB.calcExtents();

	real        bestSplitCost     = std::numeric_limits<real>::max();
	int         bestAxis          = -1;
	std::size_t bestEndpointIndex = std::numeric_limits<std::size_t>::max();
	int         axis              = nodeExtents.maxDimension();
	int         numSplitTrials    = 0;
	while(bestAxis == -1 && numSplitTrials < 3)
	{
		for(std::size_t i = 0; i < numNodeItems; ++i)
		{
			const Index   itemIndex = nodeItemIndices[i];
			const AABB3D& itemAABB  = itemAABBs[itemIndex];
			endpointsCache[axis][2 * i]     = IndexedItemEndpoint{itemAABB.getMinVertex()[axis], itemIndex, EEndpoint::MIN};
			endpointsCache[axis][2 * i + 1] = IndexedItemEndpoint{itemAABB.getMaxVertex()[axis], itemIndex, EEndpoint::MAX};
		}

		std::sort(&(endpointsCache[axis][0]), &(endpointsCache[axis][2 * numNodeItems]), 
			[](const IndexedItemEndpoint& a, const IndexedItemEndpoint& b) -> bool
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

				const real probNegative     = AABB3D(nodeAABB.getMinVertex(), endpointMaxVertex).calcSurfaceArea() * reciNodeSurfaceArea;
				const real probPositive     = AABB3D(endpointMinVertex, nodeAABB.getMaxVertex()).calcSurfaceArea() * reciNodeSurfaceArea;
				const real emptyBonus       = (numNegativeItems == 0 || numPositiveItems == 0) ? m_emptyBonus : 0.0_r;
				const real currentSplitCost = m_traversalCost + (1.0_r - emptyBonus) * m_intersectionCost * 
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
		m_nodeBuffer[nodeIndex] = IndexedKdtreeNode::makeLeaf(nodeItemIndices, numNodeItems, m_itemIndices);
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
		negativeItemIndicesCache,
		positiveItemIndicesCache + numNodeItems,
		endpointsCache);

	const std::size_t positiveChildIndex = m_numNodes;
	m_nodeBuffer[nodeIndex] = IndexedKdtreeNode::makeInner(bestSplitPos, bestAxis, positiveChildIndex);

	buildNodeRecursive(
		positiveChildIndex,
		positiveNodeAABB,
		positiveItemIndicesCache,
		numPositiveItems,
		currentNodeDepth + 1,
		newNumBadRefines,
		itemAABBs,
		negativeItemIndicesCache,
		positiveItemIndicesCache + numNodeItems,
		endpointsCache);
}

template<typename Item, typename Index>
void TIndexedKdtree<Item, Index>::getAABB(AABB3D* const out_aabb) const
{
	*out_aabb = m_rootAABB;
}

}// end namespace ph