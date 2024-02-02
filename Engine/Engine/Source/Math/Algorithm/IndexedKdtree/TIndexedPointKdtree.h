#pragma once

#include "Math/TVector3.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Math/Geometry/TAABB3D.h"
#include "Utility/TSpan.h"
#include "Utility/utility.h"

#include <Common/assertion.h>

#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <array>
#include <memory>
#include <type_traits>
#include <limits>
#include <concepts>

namespace ph::math
{

template<typename Storage, typename Item>
concept CIndexedPointKdtreeItemStorage = 
	std::default_initializable<Storage> &&
	std::copyable<Storage> &&
	requires (Storage storage)
	{
		{ storage[std::size_t{}] } -> std::convertible_to<Item>;
		{ storage.size() } -> std::convertible_to<std::size_t>;
	};

template
<
	typename Item, 
	typename Index, 
	typename PointCalculator,
	CIndexedPointKdtreeItemStorage<Item> ItemStorage = std::vector<Item>
>
class TIndexedPointKdtree final
{
	// TODO: static_assert for signature of PointCalculator

public:
	struct BuildCache final
	{
		std::vector<Index> itemIndices;
		std::vector<math::Vector3R> itemPoints;
	};

	/*! @brief Creates empty tree. Call `build()` to populate the tree.
	*/
	TIndexedPointKdtree(const std::size_t maxNodeItems, const PointCalculator& pointCalculator)
		: m_nodeBuffer     ()
		, m_numNodes       (0)
		, m_items          ()
		, m_rootAABB       ()
		, m_maxNodeItems   (maxNodeItems)
		, m_indexBuffer    ()
		, m_pointCalculator(pointCalculator)
	{
		PH_ASSERT_GT(maxNodeItems, 0);
	}

	/*! @brief Populate the tree. Better for build once then read only.
	@param items The items this tree is built for.
	*/
	void build(ItemStorage items)
	{
		BuildCache buildCache;
		build(std::move(items), buildCache);
	}

	/*! @brief Populate the tree. Better for multiple builds.
	@param items The items this tree is built for.
	@param buildCache Temporary data for the build. If a cache is reused for multiple builds,
	subsequent builds can take less time. Different trees can reuse the same cache.
	*/
	void build(ItemStorage items, BuildCache& buildCache)
	{
		m_nodeBuffer.clear();
		m_numNodes = 0;
		m_items    = std::move(items);
		m_rootAABB = math::AABB3D();
		m_indexBuffer.clear();
		if(m_items.size() == 0)
		{
			return;
		}

		auto& itemPoints = buildCache.itemPoints;
		itemPoints.resize(m_items.size());
		for(std::size_t i = 0; i < m_items.size(); ++i)
		{
			const auto& item = m_items[i];

			const math::Vector3R& center = m_pointCalculator(regular_access(item));
			itemPoints[i] = center;
		}

		PH_ASSERT_LE(m_items.size() - 1, std::numeric_limits<Index>::max());
		auto& itemIndices = buildCache.itemIndices;
		itemIndices.resize(m_items.size());
		for(std::size_t i = 0; i < m_items.size(); ++i)
		{
			itemIndices[i] = static_cast<Index>(i);
		}

		m_rootAABB = calcPointsAABB(itemIndices, itemPoints);

		buildNodeRecursive(
			0,
			m_rootAABB,
			itemIndices,
			itemPoints,
			0);
	}

	void findWithinRange(
		const math::Vector3R& location,
		const real            searchRadius,
		std::vector<Item>&    results) const
	{
		PH_ASSERT_GT(m_numNodes, 0);

		const real searchRadius2 = searchRadius * searchRadius;

		rangeTraversal(
			location, 
			searchRadius2,
			[this, location, searchRadius2, &results](const Item& item)
			{
				const math::Vector3R itemPoint = m_pointCalculator(item);
				const real           dist2     = (itemPoint - location).lengthSquared();
				if(dist2 < searchRadius2)
				{
					results.push_back(item);
				}
			});
	}

	void findNearest(
		const math::Vector3R& location,
		const std::size_t     maxItems,
		std::vector<Item>&    results) const
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
			[this, location, maxItems, 
			&searchRadius2, &numFoundItems, &isACloserThanB, &results]
			(const Item& item)
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

	template<typename ItemHandler>
	void rangeTraversal(
		const math::Vector3R& location,
		const real            squaredSearchRadius,
		ItemHandler           itemHandler) const
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

	template<typename ItemHandler>
	void nearestTraversal(
		const math::Vector3R& location,
		const real            initialSquaredSearchRadius,
		ItemHandler           itemHandler) const
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

	std::size_t numItems() const
	{
		return m_items.size();
	}

private:
	using Node = TIndexedKdtreeNode<Index, false>;

	void buildNodeRecursive(
		const std::size_t               nodeIndex,
		const math::AABB3D&             nodeAABB,
		const TSpan<Index>              nodeItemIndices,
		const TSpanView<math::Vector3R> itemPoints,
		const std::size_t               currentNodeDepth)
	{
		++m_numNodes;
		if(m_numNodes > m_nodeBuffer.size())
		{
			m_nodeBuffer.resize(m_numNodes * 2);
		}
		PH_ASSERT_LT(nodeIndex, m_nodeBuffer.size());

		if(nodeItemIndices.size() <= m_maxNodeItems)
		{
			m_nodeBuffer[nodeIndex] = Node::makeLeaf(nodeItemIndices, m_indexBuffer);
			return;
		}

		const math::Vector3R& nodeExtents = nodeAABB.getExtents();
		const auto            splitAxis   = nodeExtents.maxDimension();

		const std::size_t midIndicesIndex = nodeItemIndices.size() / 2;
		std::nth_element(
			nodeItemIndices.begin(), 
			nodeItemIndices.begin() + midIndicesIndex,
			nodeItemIndices.end(), 
			[itemPoints, splitAxis](const Index& a, const Index& b) -> bool
			{
				return itemPoints[a][splitAxis] < itemPoints[b][splitAxis];
			});

		const real splitPos = itemPoints[nodeItemIndices[midIndicesIndex]][splitAxis];

		math::Vector3R splitPosMinVertex = nodeAABB.getMinVertex();
		math::Vector3R splitPosMaxVertex = nodeAABB.getMaxVertex();
		splitPosMinVertex[splitAxis] = splitPos;
		splitPosMaxVertex[splitAxis] = splitPos;
		const math::AABB3D negativeNodeAABB(nodeAABB.getMinVertex(), splitPosMaxVertex);
		const math::AABB3D positiveNodeAABB(splitPosMinVertex, nodeAABB.getMaxVertex());
	
		buildNodeRecursive(
			nodeIndex + 1, 
			negativeNodeAABB, 
			nodeItemIndices.subspan(0, midIndicesIndex),
			itemPoints,
			currentNodeDepth + 1);

		const std::size_t positiveChildIndex = m_numNodes;
		m_nodeBuffer[nodeIndex] = Node::makeInner(splitPos, splitAxis, positiveChildIndex);

		buildNodeRecursive(
			positiveChildIndex,
			positiveNodeAABB,
			nodeItemIndices.subspan(midIndicesIndex),
			itemPoints,
			currentNodeDepth + 1);
	}

	/*! @brief Calculates AABB for points indicated by `pointIndices`.
	*/
	static math::AABB3D calcPointsAABB(
		const TSpanView<Index>          pointIndices,
		const TSpanView<math::Vector3R> points)
	{
		PH_ASSERT_GT(pointIndices.size(), 0);

		math::AABB3D pointsAABB(points[pointIndices[0]]);
		for(std::size_t i = 1; i < pointIndices.size(); ++i)
		{
			pointsAABB.unionWith(points[pointIndices[i]]);
		}
		return pointsAABB;
	}

	std::vector<Node>  m_nodeBuffer;
	std::size_t        m_numNodes;
	ItemStorage        m_items;
	math::AABB3D       m_rootAABB;
	std::size_t        m_maxNodeItems;
	std::vector<Index> m_indexBuffer;
	PointCalculator    m_pointCalculator;
};

}// end namespace ph::math
