#pragma once

#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Core/Bound/AABB3D.h"

#include <vector>
#include <utility>
#include <cstddef>

namespace ph
{

template<typename Item, typename Index, typename CenterCalculator>
class TNearestNeighborKdtree
{
public:
	using Node = TIndexedKdtreeNode<Index, false>;

	TNearestNeighborKdtree(std::size_t maxNodeItems);

	void build(std::vector<Item>&& items);

	template<typename NNResult>
	void findNearestNeighbors(
		const Vector3R& location, 
		const real      maxSearchRadius, 
		NNResult&       results) const;

private:
	std::vector<Node> m_nodes;
	std::vector<Item> m_items;
	std::size_t m_maxNodeItems;

	void buildNodeRecursive(
		std::size_t                  nodeIndex,
		const AABB3D&                nodeAABB,
		const Index*                 nodeItemIndices,
		std::size_t                  numNodeItems,
		const AABB3D&                nodeItemCentroidsAABB,
		const std::vector<Vector3R>& itemCentroids,
		std::size_t                  currentNodeDepth,
		Index*                       negativeItemIndicesCache,
		Index*                       positiveItemIndicesCache);

	AABB3D calcCentroidsAABB(
		const Index*                 itemIndices, 
		std::size_t                  numItems, 
		const std::vector<Vector3R>& itemCentroids);
};

// In-header Implementations:

template<typename Item, typename Index, typename CenterCalculator>
inline TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	TNearestNeighborKdtree(const std::size_t maxNodeItems) : 
	m_maxNodeItems(maxNodeItems)
{
	PH_ASSERT(maxNodeItems > 0);
}

template<typename Item, typename Index, typename CenterCalculator>
inline void TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	build(std::vector<Item>&& items)
{
	m_items = std::move(items);
	m_nodes.clear();
	if(m_items.empty())
	{
		return;
	}

	
}

template<typename Item, typename Index, typename CenterCalculator>
template<typename NNResult>
inline void TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	findNearestNeighbors(
		const Vector3R& location,
		const real      maxSearchRadius,
		NNResult&       results) const
{
	PH_ASSERT(k > 0);

	// TODO
}

template<typename Item, typename Index, typename CenterCalculator>
inline void TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	buildNodeRecursive(
		const std::size_t            nodeIndex,
		const AABB3D&                nodeAABB,
		const Index* const           nodeItemIndices,
		const std::size_t            numNodeItems,
		const AABB3D&                nodeItemCentroidsAABB,
		const std::vector<Vector3R>& itemCentroids,
		const std::size_t            currentNodeDepth,
		Index* const                 negativeItemIndicesCache,
		Index* const                 positiveItemIndicesCache)
{
	// TODO
}

template<typename Item, typename Index, typename CenterCalculator>
inline AABB3D TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	calcCentroidsAABB(
		const Index* const           itemIndices,
		const std::size_t            numItems,
		const std::vector<Vector3R>& itemCentroids)
{
	PH_ASSERT(itemIndices && numItems > 0);

	AABB3D centroidsAABB(itemCentroids[itemIndices[0]]);
	for(std::size_t i = 1; i < numItems; ++i)
	{
		centroidsAABB.unionWith(itemCentroids[itemIndices[i]]);
	}
	return centroidsAABB;
}

}// end namespace ph