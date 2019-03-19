#pragma once

#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Core/Bound/TAABB3D.h"
#include "Utility/utility.h"

#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <array>
#include <memory>

namespace ph
{

template<typename Item, typename Index, typename CenterCalculator>
class TCenterKdtree
{
public:
	using Node = TIndexedKdtreeNode<Index, false>;

	TCenterKdtree(std::size_t maxNodeItems, const CenterCalculator& centerCalculator);

	void build(std::vector<Item> items);

	void findWithinRange(
		const Vector3R&    location,
		real               searchRadius,
		std::vector<Item>& results) const;

	template<typename NNResult>
	void findNearestNeighbors(
		const Vector3R&    location, 
		real               maxSearchRadius, 
		NNResult&          results) const;

	std::size_t numItems() const;

private:
	std::vector<Node>  m_nodeBuffer;
	std::vector<Item>  m_items;
	AABB3D             m_rootAABB;
	std::size_t        m_numNodes;
	std::size_t        m_maxNodeItems;
	std::vector<Index> m_indexBuffer;
	CenterCalculator   m_centerCalculator;

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
inline TCenterKdtree<Item, Index, CenterCalculator>::
TCenterKdtree(const std::size_t maxNodeItems, const CenterCalculator& centerCalculator) :

	m_nodeBuffer      (),
	m_items           (),
	m_rootAABB        (),
	m_numNodes        (0),
	m_maxNodeItems    (maxNodeItems),
	m_indexBuffer     (),
	m_centerCalculator(centerCalculator)
{
	PH_ASSERT(maxNodeItems > 0);
}

template<typename Item, typename Index, typename CenterCalculator>
inline void TCenterKdtree<Item, Index, CenterCalculator>::
	build(std::vector<Item> items)
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

	std::vector<Vector3R> itemCenters(m_items.size());
	for(std::size_t i = 0; i < m_items.size(); ++i)
	{
		const auto& item = m_items[i];

		const Vector3R& center = m_centerCalculator(regular_access(item));
		itemCenters[i] = center;
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
inline void TCenterKdtree<Item, Index, CenterCalculator>::
	findWithinRange(
		const Vector3R&    location,
		const real         searchRadius,
		std::vector<Item>& results) const
{
	PH_ASSERT(m_numNodes > 0);

	const real searchRadius2 = searchRadius * searchRadius;

	constexpr std::size_t MAX_STACK_HEIGHT = 64;
	std::array<const Node*, MAX_STACK_HEIGHT> nodeStack;

	const Node* currentNode = &(m_nodeBuffer[0]);
	std::size_t stackHeight = 0;
	while(true)
	{
		PH_ASSERT(currentNode);
		if(!currentNode->isLeaf())
		{
			const int  splitAxis      = currentNode->splitAxisIndex();
			const real splitPos       = currentNode->splitPos();
			const real splitPlaneDiff = location[splitAxis] - splitPos;

			const Node* nearNode;
			const Node* farNode;
			if(splitPlaneDiff < 0)
			{
				nearNode = currentNode + 1;
				farNode  = &(m_nodeBuffer[currentNode->positiveChildIndex()]);
			}
			else
			{
				nearNode = &(m_nodeBuffer[currentNode->positiveChildIndex()]);
				farNode  = currentNode + 1;
			}

			currentNode = nearNode;
			if(searchRadius2 >= splitPlaneDiff * splitPlaneDiff)
			{
				PH_ASSERT(stackHeight < MAX_STACK_HEIGHT);
				nodeStack[stackHeight++] = farNode;
			}
		}
		// current node is leaf
		else
		{
			const std::size_t numItems          = currentNode->numItems();
			const std::size_t indexBufferOffset = currentNode->indexBufferOffset();
			for(std::size_t i = 0; i < numItems; ++i)
			{
				const Index     itemIndex  = m_indexBuffer[indexBufferOffset + i];
				const Item&     item       = m_items[itemIndex];
				const Vector3R& itemCenter = m_centerCalculator(item);
				const real      dist2      = (itemCenter - location).lengthSquared();
				if(dist2 <= searchRadius2)
				{
					results.push_back(item);
				}
			}

			if(stackHeight > 0)
			{
				currentNode = nodeStack[--stackHeight];
			}
			else
			{
				break;
			}
		}
	}// end while stackHeight > 0
}

template<typename Item, typename Index, typename CenterCalculator>
template<typename NNResult>
inline void TCenterKdtree<Item, Index, CenterCalculator>::
	findNearestNeighbors(
		const Vector3R& location,
		const real      maxSearchRadius,
		NNResult&       results) const
{
	//PH_ASSERT(k > 0);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

template<typename Item, typename Index, typename CenterCalculator>
inline void TCenterKdtree<Item, Index, CenterCalculator>::
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

	const Vector3R& nodeExtents = nodeAABB.getExtents();
	const int       splitAxis   = nodeExtents.maxDimension();

	const std::size_t midIndicesIndex = numNodeItems / 2;
	std::nth_element(
		nodeItemIndices, 
		nodeItemIndices + midIndicesIndex,
		nodeItemIndices + numNodeItems, 
		[&](const Index& a, const Index& b) -> bool
		{
			return itemCenters[a][splitAxis] < itemCenters[b][splitAxis];
		});

	const std::size_t numNegativeItems = midIndicesIndex;
	const std::size_t numPositiveItems = numNodeItems - midIndicesIndex;
	PH_ASSERT(numNegativeItems + numPositiveItems >= 2);

	const real splitPos = itemCenters[nodeItemIndices[midIndicesIndex]][splitAxis];

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
		nodeItemIndices + midIndicesIndex,
		numPositiveItems,
		itemCenters,
		currentNodeDepth + 1);
}

template<typename Item, typename Index, typename CenterCalculator>
inline AABB3D TCenterKdtree<Item, Index, CenterCalculator>::
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

template<typename Item, typename Index, typename CenterCalculator>
inline std::size_t TCenterKdtree<Item, Index, CenterCalculator>::
	numItems() const
{
	return m_items.size();
}

}// end namespace ph