#pragma once

#include "Core/Bound/AABB3D.h"
#include "Core/Accelerator/Kdtree/IndexedKdtreeNode.h"
#include "Utility/utility.h"
#include "Core/Accelerator/Kdtree/IndexedItemEndpoint.h"

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
		std::vector<Item>&& items, 
		int traversalCost, 
		int intersectionCost,
		float emptyBonus,
		std::size_t maxNodeItems);

	void build();

private:
	std::vector<Item> m_items;
	int m_traversalCost;
	int m_intersectionCost;
	float m_emptyBonus;
	std::size_t m_maxNodeItems;
	std::size_t m_maxNodeDepth;
	AABB3D m_rootAABB;
	std::vector<IndexedKdtreeNode> m_nodes;
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

	std::vector<Item>&& items,
	const int traversalCost,
	const int intersectionCost,
	const float emptyBonus,
	const std::size_t maxNodeItems) :

	m_items(std::move(items)),
	m_traversalCost(traversalCost),
	m_intersectionCost(intersectionCost),
	m_emptyBonus(emptyBonus),
	m_maxNodeItems(maxNodeItems),
	m_maxNodeDepth(0),
	m_rootAABB(),
	m_nodes(),
	m_itemIndices()
{}

template<typename Item, typename Index>
inline void TIndexedKdtree<Item, Index>::build()
{
	if(m_items.empty())
	{
		return;
	}

	m_maxNodeDepth = static_cast<std::size_t>(8 + 1.3 * std::log2(m_items.size()) + 0.5);

	std::vector<AABB3D> itemAABBs;
	regular_access(m_items.front()).calcAABB(&m_rootAABB);
	for(const auto& item : m_items)
	{
		const AABB3D aabb;
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
	if(currentNodeDepth == m_maxNodeDepth || numNodeItems <= m_maxNodeItems)
	{
		m_nodes.push_back(IndexedKdtreeNode::makeLeaf(nodeItemIndices, numNodeItems, m_itemIndices));
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
		m_nodes.push_back(IndexedKdtreeNode::makeLeaf(nodeItemIndices, numNodeItems, m_itemIndices));
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

	m_nodes.push_back(IndexedKdtreeNode::makeInner(bestSplitPos, bestAxis, m_nodes.size()));

	buildNodeRecursive(
		m_nodes.size(),
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

}// end namespace ph