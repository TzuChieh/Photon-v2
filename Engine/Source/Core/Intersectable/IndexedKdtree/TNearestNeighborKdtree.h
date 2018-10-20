#pragma once

#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Core/Bound/AABB3D.h"
#include "Utility/utility.h"

#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>

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
	std::vector<Node>  m_nodeBuffer;
	std::vector<Item>  m_items;
	AABB3D             m_rootAABB;
	std::size_t        m_numNodes;
	std::size_t        m_maxNodeItems;
	std::vector<Index> m_indexBuffer;

	void buildNodeRecursive(
		std::size_t                  nodeIndex,
		const AABB3D&                nodeAABB,
		Index*                       nodeItemIndices,
		std::size_t                  numNodeItems,
		const std::vector<Vector3R>& itemCenters,
		std::size_t                  currentNodeDepth);

	AABB3D calcCentersAABB(
		const Index*                 itemIndices, 
		std::size_t                  numItems, 
		const std::vector<Vector3R>& itemCenters) const;
};

// In-header Implementations:

template<typename Item, typename Index, typename CenterCalculator>
inline TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	TNearestNeighborKdtree(const std::size_t maxNodeItems) : 

	m_nodeBuffer(),
	m_items(),
	m_rootAABB(),
	m_numNodes(0),
	m_maxNodeItems(maxNodeItems),
	m_indexBuffer()
{
	PH_ASSERT(maxNodeItems > 0);
}

template<typename Item, typename Index, typename CenterCalculator>
inline void TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	build(std::vector<Item>&& items)
{
	m_nodeBuffer.clear();
	m_items    = std::move(items);
	m_rootAABB = AABB3D();
	m_numNodes = 0;
	m_indexBuffer.clear();
	if(m_items.empty())
	{
		return;
	}

	const CenterCalculator centerCalculator;
	std::vector<Vector3R> itemCenters;
	for(const auto& item : m_items)
	{
		const Vector3R& center = centerCalculator(regular_access(item));
		itemCenters.push_back(center);
	}

	std::unique_ptr<Index[]> itemIndices(new Index[m_items.size()]);

	PH_ASSERT(m_items.size() - 1 <= std::numeric_limits<Index>::max());
	for(std::size_t i = 0; i < m_items.size(); ++i)
	{
		itemIndices[i] = static_cast<Index>(i);
	}

	m_rootAABB = calcCentersAABB(itemIndices.get(), m_items.size(), itemCenters);

	buildNodeRecursive(
		0,
		m_rootAABB,
		itemIndices.get(),
		m_items.size(),
		itemCenters,
		0);
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
		Index* const                 nodeItemIndices,
		const std::size_t            numNodeItems,
		const std::vector<Vector3R>& itemCenters,
		const std::size_t            currentNodeDepth)
{
	++m_numNodes;
	if(m_numNodes > m_nodeBuffer.size())
	{
		m_nodeBuffer.resize(m_numNodes * 2);
	}
	PH_ASSERT(nodeIndex < m_nodeBuffer.size());

	if(numNodeItems <= m_maxNodeItems)
	{
		m_nodeBuffer[nodeIndex] = Node::makeLeaf(nodeItemIndices, numNodeItems, m_indexBuffer);
		return;
	}

	const Vector3R& nodeExtents = nodeAABB.calcExtents();
	const int       splitAxis   = nodeExtents.maxDimension();

	const std::size_t midItemIndex = numNodeItems / 2;
	std::nth_element(
		std::begin(nodeItemIndices), 
		std::begin(nodeItemIndices) + midItemIndex,
		std::begin(nodeItemIndices) + numNodeItems, 
		[&](const Index& a, const Index& b) -> bool
		{
			return itemCenters[a][splitAxis] < itemCenters[b][splitAxis];
		});

	const std::size_t numNegativeItems = midItemIndex;
	const std::size_t numPositiveItems = numNodeItems - midItemIndex;
	PH_ASSERT(numNegativeItems + numPositiveItems >= 2);

	const real splitPos = itemCenters[midItemIndex][splitAxis];

	Vector3R splitPosMinVertex = nodeAABB.getMinVertex();
	Vector3R splitPosMaxVertex = nodeAABB.getMaxVertex();
	splitPosMinVertex[splitAxis] = splitPos;
	splitPosMaxVertex[splitAxis] = splitPos;
	const AABB3D negativeNodeAABB(nodeAABB.getMinVertex(), splitPosMaxVertex);
	const AABB3D positiveNodeAABB(splitPosMinVertex, nodeAABB.getMaxVertex());
	
	buildNodeRecursive(
		nodeIndex + 1, 
		negativeNodeAABB, 
		nodeItemIndices,
		numNegativeItems,
		itemCenters,
		currentNodeDepth + 1);

	const std::size_t positiveChildIndex = m_numNodes;
	m_nodeBuffer[nodeIndex] = Node::makeInner(splitPos, splitAxis, positiveChildIndex);

	buildNodeRecursive(
		positiveChildIndex,
		positiveNodeAABB,
		nodeItemIndices,
		numPositiveItems,
		currentNodeDepth + 1);
}

template<typename Item, typename Index, typename CenterCalculator>
inline AABB3D TNearestNeighborKdtree<Item, Index, CenterCalculator>::
	calcCentersAABB(
		const Index* const           itemIndices,
		const std::size_t            numItems,
		const std::vector<Vector3R>& itemCenters) const
{
	PH_ASSERT(itemIndices && numItems > 0);

	AABB3D centersAABB(itemCenters[itemIndices[0]]);
	for(std::size_t i = 1; i < numItems; ++i)
	{
		centersAABB.unionWith(itemCenters[itemIndices[i]]);
	}
	return centersAABB;
}

}// end namespace ph