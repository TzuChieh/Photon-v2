#pragma once

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TBvhItemInfo.h"
#include "Math/Algorithm/BVH/TBvhSimdComputingContext.h"
#include "Math/Algorithm/acceleration_structure_basics.h"
#include "Utility/TArrayVector.h"
#include "Utility/TArrayStack.h"
#include "Utility/TArrayHeap.h"

#include <Common/assertion.h>
#include <Common/math_basics.h>
#include <Common/logging.h>

#include <type_traits>
#include <utility>
#include <limits>
#include <functional>
#include <cmath>

namespace ph::math
{

template<std::size_t N, typename Item, typename Index>
template<std::size_t SrcN>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::build(
	const TBvhInfoNode<SrcN, Item>* const rootNode,
	const std::size_t totalInfoNodes,
	const std::size_t totalItems)
{
#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	*this = TLinearDepthFirstWideBvh{};
	if(!rootNode)
	{
		return;
	}

	// Allocate memory for nodes and items
	m_nodes = std::make_unique<TWideBvhNode<N, Index>[]>(totalInfoNodes);
	m_items = std::make_unique<Item[]>(totalItems);

	// Can directly convert if the branch factor matches
	if constexpr(SrcN == N)
	{
		bool isSingleSplitAxisNodes = true;
		convertNodesRecursive(rootNode, isSingleSplitAxisNodes);

		m_order = isSingleSplitAxisNodes ? EBvhSplitAxisOrder::Single : EBvhSplitAxisOrder::Unbalanced;
	}
	// Try to collapse into target branch factor
	else if constexpr(is_power_of<SrcN>(N))
	{
		collapseNodesRecursive(rootNode);

		m_order = is_power_of_2(N) ? EBvhSplitAxisOrder::BalancedPow2 : EBvhSplitAxisOrder::Balanced;
	}
	else
	{
		PH_DEFAULT_LOG(Error,
			"Cannot build BVH{} from BVH{} ({}-byte index).", N, SrcN, sizeof(Index));
	}

	refitBuffer(totalInfoNodes, totalItems);
}

template<std::size_t N, typename Item, typename Index>
template<bool IS_ROBUST, typename TesterFunc>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const
{
	static_assert(CItemSegmentIntersectionTester<TesterFunc, Item>);
	static_assert(std::numeric_limits<real>::has_infinity);

#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	if(isEmpty())
	{
		return false;
	}

	PH_ASSERT(m_nodes);
	PH_ASSERT(m_items);

	switch(m_order)
	{
	// TODO: this order is not specialized/optimized yet
	case EBvhSplitAxisOrder::Unbalanced:
		return nearestTraversalGeneral<IS_ROBUST>(
			segment,
			std::forward<TesterFunc>(intersectionTester));

	// TODO: this order is not specialized/optimized yet
	case EBvhSplitAxisOrder::Balanced:
		return nearestTraversalGeneral<IS_ROBUST>(
			segment,
			std::forward<TesterFunc>(intersectionTester));

	case EBvhSplitAxisOrder::BalancedPow2:
		// This check is required, so unused code can be eliminated
		if constexpr(is_power_of_2(N))
		{
			return nearestTraversalOrdered<IS_ROBUST, EBvhSplitAxisOrder::BalancedPow2>(
				segment,
				std::forward<TesterFunc>(intersectionTester));
		}
		else
		{
			PH_DEFAULT_LOG(ErrorOnce,
				"BVH{} ({}-byte index) tried to use `BalancedPow2` order.", N, sizeof(Index));
			return false;
		}

	case EBvhSplitAxisOrder::Single:
		return nearestTraversalOrdered<IS_ROBUST, EBvhSplitAxisOrder::Single>(
			segment,
			std::forward<TesterFunc>(intersectionTester));

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<std::size_t N, typename Item, typename Index>
template<bool IS_ROBUST, typename TesterFunc>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::nearestTraversalGeneral(
	const TLineSegment<real>& segment,
	TesterFunc&& intersectionTester) const
{
#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	struct GeneralTodoNode
	{
		real minT;
		Index nodeIndex;

		bool operator > (const GeneralTodoNode& other) const
		{
			return minT > other.minT;
		}
	};

	// Traversal states

	// For general case: greedily extract minimum t using a min heap
	TArrayHeap<GeneralTodoNode, TRAVERSAL_STACK_SIZE, std::greater<>> generalTodoNodes;

	Index currentNodeIndex = 0;
	TLineSegment<real> longestSegment(segment);
	bool hasHit = false;

	// Precompute common values

	constexpr auto largestHitT = std::numeric_limits<real>::infinity();

	const auto rcpSegmentDir = segment.getDir().rcp();

	TBvhSimdComputingContext<N, Index> simdCtx;
	if constexpr(simdCtx.isSupported())
	{
		simdCtx.setSegment(segment.getOrigin(), rcpSegmentDir);
	}

	// Traverse nodes
	while(true)
	{
#if PH_PROFILE_ACCELERATION_STRUCTURES
		PH_PROFILE_NAMED_SCOPE("Traversal loop body");
#endif
		PH_ASSERT_LT(currentNodeIndex, m_numNodes);
		const NodeType& node = m_nodes[currentNodeIndex];

		std::array<real, N> hitTs;
		decltype(simdCtx.getIntersectResultAsMinTsOr(0)) simdHitTs;
		if constexpr(simdCtx.isSupported())
		{
#if PH_PROFILE_ACCELERATION_STRUCTURES
			PH_PROFILE_NAMED_SCOPE("SIMD batched AABB intersection");
#endif
			simdCtx.setNode(node);
			simdCtx.intersectAabbVolumes(longestSegment.getMinT(), longestSegment.getMaxT());
			simdHitTs = simdCtx.getIntersectResultAsMinTsOr(largestHitT);
		}
		else
		{
#if PH_PROFILE_ACCELERATION_STRUCTURES
			PH_PROFILE_NAMED_SCOPE("Batched AABB intersection");
#endif
			for(uint8 i = 0; i < N; ++i)
			{
				const auto [aabbMinT, aabbMaxT] = node.getAABB(i).isIntersectingVolume<IS_ROBUST>(
					longestSegment, rcpSegmentDir);
				hitTs[i] = aabbMinT <= aabbMaxT ? aabbMinT : largestHitT;
			}
		}

		for(uint8 i = 0; i < N; ++i)
		{
			// Default is traversing with memory order (could be improved with some sorting mechanism)
			const uint8 ci = i;

			real minT = hitTs[ci];
			if constexpr(simdCtx.isSupported())
			{
				minT = simdHitTs[ci];
			}

			if(minT < longestSegment.getMaxT())
			{
				if(node.isLeaf(ci))
				{
					const auto numItems = node.numItems(ci);
					for(std::size_t ii = 0; ii < numItems; ++ii)
					{
						const Item& item = m_items[node.getItemOffset(ci) + ii];

						const auto optHitT = intersectionTester(item, longestSegment);
						if(optHitT)
						{
							longestSegment.setMaxT(*optHitT);
							hasHit = true;
						}
					}
				}
				else
				{
					PH_ASSERT_LE(node.getChildOffset(ci), std::numeric_limits<Index>::max());
					const auto childNodeIndex = static_cast<Index>(node.getChildOffset(ci));

					generalTodoNodes.push(GeneralTodoNode{
						.minT = minT,
						.nodeIndex = childNodeIndex});
				}
			}
		}

		// Skip to a potentially closer node
		while(!generalTodoNodes.isEmpty() && generalTodoNodes.top().minT >= longestSegment.getMaxT())
		{
			generalTodoNodes.pop();
		}

		if(generalTodoNodes.isEmpty())
		{
			break;
		}
		else
		{
			currentNodeIndex = generalTodoNodes.top().nodeIndex;
			generalTodoNodes.pop();
		}
	}
	
	return hasHit;
}

template<std::size_t N, typename Item, typename Index>
template<bool IS_ROBUST, EBvhSplitAxisOrder ORDER, typename TesterFunc>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::nearestTraversalOrdered(
	const TLineSegment<real>& segment,
	TesterFunc&& intersectionTester) const
{
#if PH_PROFILE_ACCELERATION_STRUCTURES
	PH_PROFILE_SCOPE();
#endif

	// Traversal states

	// Add pending nodes to a stack
	TArrayStack<Index, TRAVERSAL_STACK_SIZE> todoNodes;

	Index currentNodeIndex = 0;
	TLineSegment<real> longestSegment(segment);
	bool hasHit = false;

	// Precompute common values

	constexpr auto largestHitT = std::numeric_limits<real>::infinity();

	const auto rcpSegmentDir = segment.getDir().rcp();

	std::array<uint8, 3> isNegDir;
	if constexpr(
		ORDER == EBvhSplitAxisOrder::Single ||
		ORDER == EBvhSplitAxisOrder::BalancedPow2)
	{
		isNegDir = {
			segment.getDir().x() < 0,
			segment.getDir().y() < 0,
			segment.getDir().z() < 0};
	}

	TBvhSimdComputingContext<N, Index> simdCtx;
	if constexpr(simdCtx.isSupported())
	{
		simdCtx.setSegment(segment.getOrigin(), rcpSegmentDir);
	}

	// Traverse nodes
	while(true)
	{
#if PH_PROFILE_ACCELERATION_STRUCTURES
		PH_PROFILE_NAMED_SCOPE("Traversal loop body");
#endif
		PH_ASSERT_LT(currentNodeIndex, m_numNodes);
		const NodeType& node = m_nodes[currentNodeIndex];

		std::array<real, N> hitTs;
		decltype(simdCtx.getIntersectResultAsMinTsOr(0)) simdHitTs;
		if constexpr(simdCtx.isSupported())
		{
#if PH_PROFILE_ACCELERATION_STRUCTURES
			PH_PROFILE_NAMED_SCOPE("SIMD batched AABB intersection");
#endif
			simdCtx.setNode(node);
			simdCtx.intersectAabbVolumes(longestSegment.getMinT(), longestSegment.getMaxT());
			simdHitTs = simdCtx.getIntersectResultAsMinTsOr(largestHitT);
		}
		else
		{
#if PH_PROFILE_ACCELERATION_STRUCTURES
			PH_PROFILE_NAMED_SCOPE("Batched AABB intersection");
#endif
			for(uint8 i = 0; i < N; ++i)
			{
				const auto [aabbMinT, aabbMaxT] = node.getAABB(i).isIntersectingVolume<IS_ROBUST>(
					longestSegment, rcpSegmentDir);
				hitTs[i] = aabbMinT <= aabbMaxT ? aabbMinT : largestHitT;
			}
		}

		std::array<uint8, N> orderTable;

		// All children have the same split axis, traverse from the nearest one
		if constexpr(ORDER == EBvhSplitAxisOrder::Single)
		{
			constexpr auto table = makeSingleOrderTable();
			orderTable = table[isNegDir[node.getSplitAxis(0)]];
		}
		// Children are split by balanced axes (and N is power-of-2)
		else if constexpr(ORDER == EBvhSplitAxisOrder::BalancedPow2)
		{
			constexpr auto table = makeBalancedPow2OrderTable();

			uint32 permutationIdx = 0;
			for(uint32 si = 0; si < N - 1; ++si)
			{
				permutationIdx <<= 1;
				permutationIdx |= isNegDir[node.getSplitAxis(si)];
			}

			orderTable = table[permutationIdx];
		}
		else
		{
			constexpr auto table = makeIdentityOrderTable();
			orderTable = table;
		}

		std::array<Index, N> nextChildNodes;
		std::size_t numNextChildNodes = 0;
		for(uint8 i = 0; i < N; ++i)
		{
			// Get the permuted child index
			const uint8 ci = orderTable[i];

			real minT = hitTs[ci];
			if constexpr(simdCtx.isSupported())
			{
				minT = simdHitTs[ci];
			}

			if(minT < longestSegment.getMaxT())
			{
				if(node.isLeaf(ci))
				{
					const auto numItems = node.numItems(ci);
					for(std::size_t ii = 0; ii < numItems; ++ii)
					{
						const Item& item = m_items[node.getItemOffset(ci) + ii];

						const auto optHitT = intersectionTester(item, longestSegment);
						if(optHitT)
						{
							longestSegment.setMaxT(*optHitT);
							hasHit = true;
						}
					}
				}
				else
				{
					PH_ASSERT_LE(node.getChildOffset(ci), std::numeric_limits<Index>::max());
					const auto childNodeIndex = static_cast<Index>(node.getChildOffset(ci));

					nextChildNodes[numNextChildNodes] = childNodeIndex;
					++numNextChildNodes;
				}
			}
		}

		// Push nodes to stack such that the nearest one is on top
		while(numNextChildNodes > 0)
		{
			--numNextChildNodes;
			todoNodes.push(nextChildNodes[numNextChildNodes]);
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
	
	return hasHit;
}

template<std::size_t N, typename Item, typename Index>
inline bool TLinearDepthFirstWideBvh<N, Item, Index>
::isEmpty() const
{
	return m_numNodes == 0;
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::getRoot() const
-> const NodeType&
{
	PH_ASSERT(m_nodes);
	PH_ASSERT_GT(m_numNodes, 0);
	return m_nodes[0];
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::numNodes() const
-> std::size_t
{
	return m_numNodes;
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::numItems() const
-> std::size_t
{
	return m_numItems;
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::memoryUsage() const
-> std::size_t
{
	return sizeof(*this) + m_numNodes * sizeof(NodeType) + m_numItems * sizeof(Item);
}

template<std::size_t N, typename Item, typename Index>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::balancedPow2OrderTableToString() const
-> std::string
{
	constexpr auto table = makeBalancedPow2OrderTable();

	std::string str = "\n";
	for(std::size_t i = 0; i < BALANCED_POW2_ORDER_TABLE_SIZE; ++i)
	{
		for(std::size_t j = 0; j < N; ++j)
		{
			str += std::to_string(table[i][j]);
			str += j < N - 1 ? ", " : "\n";
		}
	}
	return str;
}

template<std::size_t N, typename Item, typename Index>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::convertNodesRecursive(
	const TBvhInfoNode<N, Item>* const infoNode,
	bool& isSingleSplitAxisNodes)
{
	PH_ASSERT(infoNode);

	// Basically this will only get called once (if the whole tree is a single leaf)
	if(infoNode->isLeaf())
	{
		const auto itemOffset = m_numItems;
		for(std::size_t i = 0; i < infoNode->getItems().size(); ++i)
		{
			m_items[itemOffset + i] = infoNode->getItems()[i].item;
		}
		m_numItems += static_cast<Index>(infoNode->getItems().size());

		// Does not matter as there is only a single child
		constexpr std::size_t singleLeafSplitAxis = constant::X_AXIS;

		m_nodes[m_numNodes].setLeaf(
			0,
			infoNode->getAABB(),
			itemOffset,
			singleLeafSplitAxis,
			infoNode->getItems().size());
		PH_ASSERT_LT(m_numNodes, m_numNodes + 1);// overflow check
		m_numNodes += 1;
		return;
	}

	PH_ASSERT(infoNode->isInternal());

	isSingleSplitAxisNodes &= infoNode->isSingleSplitAxis();

	const auto nodeOffset = m_numNodes;
	NodeType* const node = &(m_nodes[nodeOffset]);
	PH_ASSERT_LT(m_numNodes, m_numNodes + 1);// overflow check
	m_numNodes += 1;

	for(std::size_t ci = 0; ci < infoNode->numChildren(); ++ci)
	{
		const TBvhInfoNode<N, Item>* const childInfoNode = infoNode->getChild(ci);
		const auto splitAxis = infoNode->getSplitAxis(ci);
		if(!childInfoNode)
		{
			node->setEmptyLeaf(ci, splitAxis);
			continue;
		}
		
		if(childInfoNode->isLeaf())
		{
			const auto itemOffset = m_numItems;
			for(std::size_t i = 0; i < childInfoNode->getItems().size(); ++i)
			{
				m_items[itemOffset + i] = childInfoNode->getItems()[i].item;
			}
			m_numItems += static_cast<Index>(childInfoNode->getItems().size());

			node->setLeaf(
				ci,
				childInfoNode->getAABB(),
				itemOffset,
				splitAxis,
				childInfoNode->getItems().size());
		}
		else if(childInfoNode->isInternal())
		{
			const auto childNodeOffset = m_numNodes;
			convertNodesRecursive(childInfoNode, isSingleSplitAxisNodes);

			node->setInternal(
				ci,
				childInfoNode->getAABB(),
				childNodeOffset,
				splitAxis);
		}
		else
		{
			PH_ASSERT_UNREACHABLE_SECTION();
		}
	}
}

template<std::size_t N, typename Item, typename Index>
template<std::size_t SrcN>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::collapseNodesRecursive(
	const TBvhInfoNode<SrcN, Item>* const infoNode)
{
	PH_ASSERT(infoNode);

	TCollapsedNodes<SrcN> collapsedNodes = collapseToSingleLevel<SrcN>(infoNode);

	constexpr auto emptyNodes = TCollapsedNodes<SrcN>{}.nodes;
	if(collapsedNodes.nodes == emptyNodes)
	{
		return;
	}

	const auto nodeIndex = m_numNodes;
	NodeType* const node = &(m_nodes[nodeIndex]);
	PH_ASSERT_LT(m_numNodes, m_numNodes + 1);// overflow check
	m_numNodes += 1;

	for(std::size_t ci = 0; ci < N; ++ci)
	{
		const TBvhInfoNode<SrcN, Item>* collapsedNode = collapsedNodes.nodes[ci];
		const auto collapsedSplitAxis = collapsedNodes.splitAxes[ci];
		if(collapsedNode)
		{
			if(collapsedNode->isLeaf())
			{
				const auto itemOffset = m_numItems;
				for(std::size_t i = 0; i < collapsedNode->getItems().size(); ++i)
				{
					m_items[itemOffset + i] = collapsedNode->getItems()[i].item;
				}
				m_numItems += static_cast<Index>(collapsedNode->getItems().size());

				node->setLeaf(
					ci,
					collapsedNode->getAABB(),
					itemOffset,
					collapsedSplitAxis,
					collapsedNode->getItems().size());
			}
			else if(collapsedNode->isInternal())
			{
				const auto childNodeIndex = m_numNodes;
				collapseNodesRecursive(collapsedNode);

				node->setInternal(
					ci,
					collapsedNode->getAABB(),
					childNodeIndex,
					collapsedSplitAxis);
			}
		}
		else
		{
			node->setEmptyLeaf(
				ci,
				collapsedSplitAxis);
		}
	}
}

template<std::size_t N, typename Item, typename Index>
inline void TLinearDepthFirstWideBvh<N, Item, Index>
::refitBuffer(const std::size_t nodeBufferSize, const std::size_t itemBufferSize)
{
	// For potentially better performance on buffer content copying
	static_assert(std::is_trivially_copy_assignable_v<NodeType>);

	PH_ASSERT_LE(m_numNodes, nodeBufferSize);
	PH_ASSERT_LE(m_numItems, itemBufferSize);

	if(m_numNodes != nodeBufferSize)
	{
		auto nodes = std::make_unique<NodeType[]>(m_numNodes);
		std::copy_n(m_nodes.get(), m_numNodes, nodes.get());

		m_nodes = std::move(nodes);
	}

	if(m_numItems != itemBufferSize)
	{
		auto items = std::make_unique<Item[]>(m_numItems);
		std::copy_n(m_items.get(), m_numItems, items.get());

		m_items = std::move(items);
	}
}

template<std::size_t N, typename Item, typename Index>
template<std::size_t SrcN>
inline auto TLinearDepthFirstWideBvh<N, Item, Index>
::collapseToSingleLevel(
	const TBvhInfoNode<SrcN, Item>* const infoNode)
-> TCollapsedNodes<SrcN>
{
	PH_ASSERT(infoNode);

	constexpr auto numLevels = numTreeletLevels<SrcN>();

	// Collapse the treelet under `infoNode` into a single level (with BFS)

	TArrayVector<const TBvhInfoNode<SrcN, Item>*, N> levelNodes;
	levelNodes.pushBack(infoNode);

	TCollapsedNodes<SrcN> collapsedNodes{};
	std::size_t nextSplitAxisIdx = 0;
	for(std::size_t levelIdx = 0; levelIdx < numLevels; ++levelIdx)
	{
		const bool isFinalLevel = levelIdx + 1 == numLevels;
		if(isFinalLevel)
		{
			PH_ASSERT_EQ(levelNodes.size(), N);
			for(std::size_t li = 0; li < N; ++li)
			{
				collapsedNodes.nodes[li] = levelNodes[li];
			}
		}
		else
		{
			constexpr auto noAxis = infoNode->noAxisFlag();

			TArrayVector<const TBvhInfoNode<SrcN, Item>*, N> childNodes;
			for(const TBvhInfoNode<SrcN, Item>* parentNode : levelNodes)
			{
				for(std::size_t ci = 0; ci < SrcN; ++ci)
				{
					const bool isLastChild = ci + 1 == SrcN;

					// Obtain split axis
					if(!isLastChild)
					{
						// Does not matter which axis is chosen for empty and leaf nodes
						auto splitAxis = parentNode && parentNode->isInternal()
							? parentNode->getSplitAxis(ci) : noAxis;
						splitAxis = splitAxis != noAxis
							? splitAxis : constant::X_AXIS;

						collapsedNodes.splitAxes[nextSplitAxisIdx] = splitAxis;
						++nextSplitAxisIdx;
					}
					
					// Obtain child node
					{
						const TBvhInfoNode<SrcN, Item>* childNode = nullptr;
						if(parentNode)
						{
							if(parentNode->isInternal())
							{
								childNode = parentNode->getChild(ci);
							}
							else if(parentNode->isLeaf())
							{
								// Forward leaf node to next level (only do this for the first child
								// index so we will not have duplicated leaf nodes)
								childNode = ci == 0 ? parentNode : nullptr;
							}
						}

						childNodes.pushBack(childNode);
					}
				}
			}// end for each node in this level

			levelNodes = childNodes;
		}
	}// end for each level

	// Similar to info node, at most `N - 1` split axis entries will be used
	PH_ASSERT_LE(nextSplitAxisIdx + 1, N);

	return collapsedNodes;
}

template<std::size_t N, typename Item, typename Index>
template<std::size_t SrcN>
inline constexpr auto TLinearDepthFirstWideBvh<N, Item, Index>
::numTreeletLevels()
-> std::size_t
{
	std::size_t numLevels = 1;
	{
		static_assert(is_power_of<SrcN>(N));

		std::size_t n = 1;
		while(n < N)
		{
			n *= SrcN;
			++numLevels;
		}
	}

	return numLevels;
}

template<std::size_t N, typename Item, typename Index>
inline constexpr auto TLinearDepthFirstWideBvh<N, Item, Index>
::makeIdentityOrderTable()
-> std::array<uint8, N>
{
	static_assert(N - 1 <= std::numeric_limits<uint8>::max());

	std::array<uint8, N> table;
	for(std::size_t i = 0; i < N; ++i)
	{
		table[i] = static_cast<uint8>(i);
	}
	return table;
}

template<std::size_t N, typename Item, typename Index>
inline constexpr auto TLinearDepthFirstWideBvh<N, Item, Index>
::makeSingleOrderTable()
-> std::array<std::array<uint8, N>, 2>
{
	static_assert(N - 1 <= std::numeric_limits<uint8>::max());

	std::array<std::array<uint8, N>, 2> table;
	for(std::size_t isNegDir = 0; isNegDir < 2; ++isNegDir)
	{
		for(std::size_t i = 0; i < N; ++i)
		{
			// Negative segment direction: use reversed child order
			if(isNegDir)
			{
				table[isNegDir][i] = static_cast<uint8>(N - 1 - i);
			}
			// Positive segment direction: use original child order
			else
			{
				table[isNegDir][i] = static_cast<uint8>(i);
			}
		}
	}
	return table;
}

template<std::size_t N, typename Item, typename Index>
inline constexpr auto TLinearDepthFirstWideBvh<N, Item, Index>
::makeBalancedPow2OrderTable()
-> std::array<std::array<uint8, N>, BALANCED_POW2_ORDER_TABLE_SIZE>
{
	static_assert(is_power_of_2(N));
	static_assert(N - 1 <= std::numeric_limits<uint8>::max());

	constexpr auto reverseRange =
		[](const std::size_t begin, const std::size_t end, std::array<uint8, N>& permutation)
		{
			const auto size = end - begin;
			const auto halfSize = size / 2;

			// Cache first half of the range
			std::array<uint8, N> temp;
			for(std::size_t i = 0; i < halfSize; ++i)
			{
				temp[i] = permutation[begin + i];
			}

			// Copy second half of the range to first half
			for(std::size_t i = 0; i < halfSize; ++i)
			{
				permutation[begin + i] = permutation[begin + halfSize + i];
			}

			// Copy cached first half of the range to second half
			for(std::size_t i = 0; i < halfSize; ++i)
			{
				permutation[begin + halfSize + i] = temp[i];
			}
		};

	std::array<std::array<uint8, N>, BALANCED_POW2_ORDER_TABLE_SIZE> table;
	table.fill(makeIdentityOrderTable());

	// Minus one since the bottom level has no split axis
	constexpr auto numLevels = numTreeletLevels<2>() - 1;
	constexpr auto numDirBits = N - 1;

	// Reverse child index permutations for each level recursively (from bottom to top, otherwise
	// upper levels will affect the order of bottom level, which is more difficult to track)
	for(std::size_t entryIdx = 0; entryIdx < table.size(); ++entryIdx)
	{
		std::size_t numSplitsInLevel = N / 2;
		std::size_t splitIdxInLevel = 0;
		std::size_t permutationRange = 2;
		for(std::size_t bi = 0; bi < numDirBits; ++bi)
		{
			const auto bit = (entryIdx >> bi) & 0b1;

			// Negative segment direction: use reversed child order
			if(bit)
			{
				reverseRange(
					permutationRange * (numSplitsInLevel - splitIdxInLevel - 1),
					permutationRange * (numSplitsInLevel - splitIdxInLevel),
					table[entryIdx]);
			}
			// Positive segment direction: use original child order
			else
			{}

			++splitIdxInLevel;
			if(splitIdxInLevel == numSplitsInLevel)
			{
				numSplitsInLevel /= 2;
				splitIdxInLevel = 0;
				permutationRange *= 2;
			}
		}
	}

	return table;
}

}// end namespace ph::math
