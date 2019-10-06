#pragma once

#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Math/Geometry/TAABB3D.h"
#include "Utility/utility.h"

#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <array>
#include <memory>
#include <type_traits>
#include <limits>

namespace ph
{

// TODO: static_assert for signature of PointCalculator

template<typename Item, typename Index, typename PointCalculator>
class TIndexedPointKdtree
{
public:
	using Node   = TIndexedKdtreeNode<Index, false>;
	using AABB3D = math::AABB3D;

	TIndexedPointKdtree(std::size_t maxNodeItems, const PointCalculator& pointCalculator);

	void build(std::vector<Item> items);

	void findWithinRange(
		const Vector3R&    location,
		real               searchRadius,
		std::vector<Item>& results) const;

	void findNearest(
		const Vector3R&    location,
		std::size_t        maxItems,
		std::vector<Item>& results) const;

	template<typename ItemHandler>
	void rangeTraversal(
		const Vector3R& location,
		real            squaredSearchRadius,
		ItemHandler     itemHandler) const;

	template<typename ItemHandler>
	void nearestTraversal(
		const Vector3R& location,
		real            initialSquaredSearchRadius,
		ItemHandler     itemHandler) const;

	std::size_t numItems() const;

private:
	std::vector<Node>  m_nodeBuffer;
	std::vector<Item>  m_items;
	AABB3D             m_rootAABB;
	std::size_t        m_numNodes;
	std::size_t        m_maxNodeItems;
	std::vector<Index> m_indexBuffer;
	PointCalculator    m_pointCalculator;

	void buildNodeRecursive(
		std::size_t                  nodeIndex,
		const AABB3D&                nodeAABB,
		Index*                       nodeItemIndices,
		std::size_t                  numNodeItems,
		const std::vector<Vector3R>& itemPoints,
		std::size_t                  currentNodeDepth);

	static AABB3D calcPointsAABB(
		const Index*                 pointIndices, 
		std::size_t                  numPoints, 
		const std::vector<Vector3R>& pointBuffer);
};

// In-header Implementations:

template<typename Item, typename Index, typename PointCalculator>
inline TIndexedPointKdtree<Item, Index, PointCalculator>::
	TIndexedPointKdtree(const std::size_t maxNodeItems, const PointCalculator& pointCalculator) :

	m_nodeBuffer     (),
	m_items          (),
	m_rootAABB       (),
	m_numNodes       (0),
	m_maxNodeItems   (maxNodeItems),
	m_indexBuffer    (),
	m_pointCalculator(pointCalculator)
{
	PH_ASSERT_GT(maxNodeItems, 0);
}

template<typename Item, typename Index, typename PointCalculator>
inline void TIndexedPointKdtree<Item, Index, PointCalculator>::
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

	std::vector<Vector3R> itemPoints(m_items.size());
	for(std::size_t i = 0; i < m_items.size(); ++i)
	{
		const auto& item = m_items[i];

		const Vector3R& center = m_pointCalculator(regular_access(item));
		itemPoints[i] = center;
	}

	std::unique_ptr<Index[]> itemIndices(new Index[m_items.size()]);

	PH_ASSERT(m_items.size() - 1 <= std::numeric_limits<Index>::max());
	for(std::size_t i = 0; i < m_items.size(); ++i)
	{
		itemIndices[i] = static_cast<Index>(i);
	}

	m_rootAABB = calcPointsAABB(itemIndices.get(), m_items.size(), itemPoints);

	buildNodeRecursive(
		0,
		m_rootAABB,
		itemIndices.get(),
		m_items.size(),
		itemPoints,
		0);
}

template<typename Item, typename Index, typename PointCalculator>
inline void TIndexedPointKdtree<Item, Index, PointCalculator>::
	findWithinRange(
		const Vector3R&    location,
		const real         searchRadius,
		std::vector<Item>& results) const
{
	PH_ASSERT_GT(m_numNodes, 0);

	const real searchRadius2 = searchRadius * searchRadius;

	rangeTraversal(
		location, 
		searchRadius2,
		[this, location, searchRadius2, &results](const Item& item)
		{
			const Vector3R itemPoint = m_pointCalculator(item);
			const real     dist2     = (itemPoint - location).lengthSquared();
			if(dist2 < searchRadius2)
			{
				results.push_back(item);
			}
		});
}

template<typename Item, typename Index, typename PointCalculator>
inline void TIndexedPointKdtree<Item, Index, PointCalculator>::
	findNearest(
		const Vector3R&    location,
		const std::size_t  maxItems,
		std::vector<Item>& results) const
{
	PH_ASSERT_GT(m_numNodes, 0);
	PH_ASSERT_GT(maxItems, 0);

	// OPT: distance calculation can be cached

	auto isACloserThanB = 
		[this, location](const Item& itemA, const Item& itemB) -> bool
		{
			return (m_pointCalculator(itemA) - location).lengthSquared() < 
			       (m_pointCalculator(itemB) - location).lengthSquared();
		};

	real        searchRadius2 = std::numeric_limits<real>::max();
	std::size_t numFoundItems = 0;
	auto handler = 
		[this, location, maxItems, &searchRadius2, &numFoundItems, &isACloserThanB, &results](const Item& item)
		{
			/*
				If k nearest neighbors are required and n items are processed
				by this handler, this handler (not including traversal) will
				take O(k + (n-k)*log(k)) time in total.
			*/

			// output buffer is not full, just insert the item
			if(numFoundItems < maxItems)
			{
				results.push_back(item);
				numFoundItems++;

				// once output buffer is full, make it a max heap
				if(numFoundItems == maxItems)
				{
					// this takes O(k) time
					std::make_heap(
						results.end() - maxItems, 
						results.end(), 
						isACloserThanB);

					// the furthest one is at the max heap's root
					const Item& furthestItem = results[results.size() - maxItems];

					// search radius can be shrunk in this case
					searchRadius2 = (m_pointCalculator(furthestItem) - location).lengthSquared();
				}
			}
			// last <maxItems> items in output buffer forms a max heap now
			else
			{
				// the furthest one is at the max heap's root
				const Item& furthestItem = results[results.size() - maxItems];

				if(isACloserThanB(item, furthestItem))
				{
					// remove furthest item, this takes O(log(k)) time
					std::pop_heap(
						results.end() - maxItems,
						results.end(),
						isACloserThanB);

					results.back() = item;

					// add new item, this takes O(log(k)) time
					std::push_heap(
						results.end() - maxItems,
						results.end(),
						isACloserThanB);

					// search radius can be shrunk in this case
					searchRadius2 = (m_pointCalculator(furthestItem) - location).lengthSquared();
				}
			}

			return searchRadius2;
		};

	nearestTraversal(
		location, 
		searchRadius2,
		handler);
}

template<typename Item, typename Index, typename PointCalculator>
template<typename ItemHandler>
inline void TIndexedPointKdtree<Item, Index, PointCalculator>::
	rangeTraversal(
		const Vector3R& location,
		const real      squaredSearchRadius,
		ItemHandler     itemHandler) const
{
	static_assert(std::is_invocable_v<ItemHandler, Item>,
		"ItemHandler must accept an item as input.");

	PH_ASSERT_GT(m_numNodes, 0);
	PH_ASSERT_LE(squaredSearchRadius, std::numeric_limits<real>::max());

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
			if(squaredSearchRadius >= splitPlaneDiff * splitPlaneDiff)
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
				const Index itemIndex = m_indexBuffer[indexBufferOffset + i];
				const Item& item      = m_items[itemIndex];

				itemHandler(item);
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

template<typename Item, typename Index, typename PointCalculator>
template<typename ItemHandler>
inline void TIndexedPointKdtree<Item, Index, PointCalculator>::
	nearestTraversal(
		const Vector3R& location,
		const real      initialSquaredSearchRadius,
		ItemHandler     itemHandler) const
{
	static_assert(std::is_invocable_v<ItemHandler, Item>,
		"ItemHandler must accept an item as input.");

	using Return = decltype(itemHandler(std::declval<Item>()));
	static_assert(std::is_same_v<Return, real>,
		"ItemHandler must return an potentially shrunk squared search radius.");

	PH_ASSERT_GT(m_numNodes, 0);
	PH_ASSERT_LE(initialSquaredSearchRadius, std::numeric_limits<real>::max());

	struct NodeRecord
	{
		const Node* node;

		// The value is zero for near nodes as they should not be skipped by distance test.
		real        parentSplitPlaneDiff2;
	};

	constexpr std::size_t MAX_STACK_HEIGHT = 64;
	std::array<NodeRecord, MAX_STACK_HEIGHT> nodeStack;

	NodeRecord  currentNode    = {&(m_nodeBuffer[0]), 0};
	std::size_t stackHeight    = 0;
	real        currentRadius2 = initialSquaredSearchRadius;
	while(true)
	{
		PH_ASSERT(currentNode.node);
		if(!currentNode.node->isLeaf())
		{
			const int  splitAxis      = currentNode.node->splitAxisIndex();
			const real splitPos       = currentNode.node->splitPos();
			const real splitPlaneDiff = location[splitAxis] - splitPos;

			const Node* nearNode;
			const Node* farNode;
			if(splitPlaneDiff < 0)
			{
				nearNode = currentNode.node + 1;
				farNode  = &(m_nodeBuffer[currentNode.node->positiveChildIndex()]);
			}
			else
			{
				nearNode = &(m_nodeBuffer[currentNode.node->positiveChildIndex()]);
				farNode  = currentNode.node + 1;
			}

			const real splitPlaneDiff2 = splitPlaneDiff * splitPlaneDiff;

			currentNode = {nearNode, 0};
			if(currentRadius2 >= splitPlaneDiff2)
			{
				PH_ASSERT(stackHeight < MAX_STACK_HEIGHT);
				nodeStack[stackHeight++] = {farNode, splitPlaneDiff2};
			}
		}
		// current node is leaf
		else
		{
			// For far nodes, they can be culled if radius has shrunk.
			// For near nodes, they have <parentSplitPlaneDiff2> == 0 hence cannot be skipped.
			if(currentRadius2 >= currentNode.parentSplitPlaneDiff2)
			{
				const std::size_t numItems          = currentNode.node->numItems();
				const std::size_t indexBufferOffset = currentNode.node->indexBufferOffset();
				for(std::size_t i = 0; i < numItems; ++i)
				{
					const Index itemIndex = m_indexBuffer[indexBufferOffset + i];
					const Item& item      = m_items[itemIndex];

					// potentially reduce search radius
					const real shrunkRadius2 = itemHandler(item);
					PH_ASSERT_LE(shrunkRadius2, currentRadius2);
					currentRadius2 = shrunkRadius2;
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

template<typename Item, typename Index, typename PointCalculator>
inline void TIndexedPointKdtree<Item, Index, PointCalculator>::
	buildNodeRecursive(
		const std::size_t            nodeIndex,
		const AABB3D&                nodeAABB,
		Index* const                 nodeItemIndices,
		const std::size_t            numNodeItems,
		const std::vector<Vector3R>& itemPoints,
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
			return itemPoints[a][splitAxis] < itemPoints[b][splitAxis];
		});

	const std::size_t numNegativeItems = midIndicesIndex;
	const std::size_t numPositiveItems = numNodeItems - midIndicesIndex;
	PH_ASSERT(numNegativeItems + numPositiveItems >= 2);

	const real splitPos = itemPoints[nodeItemIndices[midIndicesIndex]][splitAxis];

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
		itemPoints,
		currentNodeDepth + 1);

	const std::size_t positiveChildIndex = m_numNodes;
	m_nodeBuffer[nodeIndex] = Node::makeInner(splitPos, splitAxis, positiveChildIndex);

	buildNodeRecursive(
		positiveChildIndex,
		positiveNodeAABB,
		nodeItemIndices + midIndicesIndex,
		numPositiveItems,
		itemPoints,
		currentNodeDepth + 1);
}

template<typename Item, typename Index, typename PointCalculator>
inline std::size_t TIndexedPointKdtree<Item, Index, PointCalculator>::
	numItems() const
{
	return m_items.size();
}

template<typename Item, typename Index, typename PointCalculator>
inline auto TIndexedPointKdtree<Item, Index, PointCalculator>::
	calcPointsAABB(
		const Index*                 pointIndices,
		const std::size_t            numPoints,
		const std::vector<Vector3R>& pointBuffer)
	-> AABB3D
{
	PH_ASSERT(pointIndices && numPoints > 0);

	AABB3D pointsAABB(pointBuffer[pointIndices[0]]);
	for(std::size_t i = 1; i < numPoints; ++i)
	{
		pointsAABB.unionWith(pointBuffer[pointIndices[i]]);
	}
	return pointsAABB;
}

}// end namespace ph