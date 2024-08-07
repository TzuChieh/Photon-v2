#pragma once

#include "Math/Algorithm/BVH/TBvhBuilder.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TLinearBvhNode.h"
#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.h"
#include "Math/math.h"
#include "Core/Intersection/Intersectable.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <cmath>
#include <array>

namespace ph::math
{

template<typename Item, typename ItemToAABB>
inline TBvhBuilder<Item, ItemToAABB>::TBvhBuilder(
	const EBvhNodeSplitMethod splitMethod,
	ItemToAABB itemToAABB,
	BvhParams params)

	: m_infoBuffer()
	, m_infoNodes()
	, m_params(params)
	, m_itemToAABB(std::move(itemToAABB))
	, m_splitMethod(splitMethod)
{}

template<typename Item, typename ItemToAABB>
inline auto TBvhBuilder<Item, ItemToAABB>
::buildInformativeBinaryBvh(TSpanView<Item> items)
-> const InfoNode*
{
	clearBuildData();

	m_infoBuffer.resize(items.size());
	for(std::size_t i = 0; i < items.size(); i++)
	{
		m_infoBuffer[i] = ItemInfo(items[i]);
	}

	// To have stable node pointers, pre-allocate enough nodes beforehand. We can calculate maximum
	// nodes required for a BVH from the number of items.
	const std::size_t numLeaves = ceil_div(items.size(), m_params.getMaxNodeItems());
	const std::size_t maxNodes  = 2 * numLeaves - 1;
	m_infoNodes.reserve(maxNodes);

	const InfoNode* rootNode = nullptr;
	switch(m_splitMethod)
	{
	case EBvhNodeSplitMethod::EqualItems:
		rootNode = buildBinaryBvhInfoNodeRecursive<EBvhNodeSplitMethod::EqualItems>(
			m_infoBuffer);
		break;

	case EBvhNodeSplitMethod::SAH_Buckets:
		rootNode = buildBinaryBvhInfoNodeRecursive<EBvhNodeSplitMethod::SAH_Buckets>(
			m_infoBuffer);
		break;

	default:
		PH_DEFAULT_LOG(Warning,
			"at `BvhBuilder::buildInformativeBinaryBvh()`, unsupported BVH split method");
		break;
	}

	PH_ASSERT_EQ(m_infoBuffer.size(), items.size());
	PH_ASSERT_LE(m_infoNodes.size(), maxNodes);

	// Verify the nodes by doing a full traversal
	PH_ASSERT_EQ(m_infoNodes.size(), calcTotalNodes(rootNode));
	PH_ASSERT_EQ(m_infoBuffer.size(), calcTotalItems(rootNode));

	return rootNode;
}

template<typename Item, typename ItemToAABB>
template<typename IndexType>
inline void TBvhBuilder<Item, ItemToAABB>
::buildLinearDepthFirstBinaryBvh(
	const InfoNode* const rootNode,
	TLinearDepthFirstBinaryBvh<Item, IndexType>* const out_bvh)
{
	PH_ASSERT(rootNode);
	PH_ASSERT(out_bvh);

	// Item infos and nodes are already in the desired order

	// Allocate memory for nodes and items
	out_bvh->nodes = std::make_unique<TLinearBvhNode<Item>[]>(m_infoNodes.size());
	out_bvh->items = std::make_unique<Item[]>(m_infoBuffer.size());

	// Flatten the info tree into a more compact representation
	buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode, out_bvh);

	PH_ASSERT_EQ(out_bvh->numNodes, m_infoNodes.size());
	PH_ASSERT_EQ(out_bvh->numItems, m_infoBuffer.size());
}

template<typename Item, typename ItemToAABB>
inline void TBvhBuilder<Item, ItemToAABB>
::clearBuildData()
{
	m_infoBuffer.clear();
	m_infoNodes.clear();
}

template<typename Item, typename ItemToAABB>
template<EBvhNodeSplitMethod SPLIT_METHOD>
inline auto TBvhBuilder<Item, ItemToAABB>
::buildBinaryBvhInfoNodeRecursive(
	const TSpan<ItemInfo> itemInfos)
-> const InfoNode*
{
	// Creating a new node must not cause reallocation
	PH_ASSERT_LT(m_infoNodes.size(), m_infoNodes.capacity());

	m_infoNodes.push_back(InfoNode{});
	InfoNode* const node = &m_infoNodes.back();

	AABB3D nodeAABB(itemInfos.empty() ? AABB3D(Vector3R(0)) : itemInfos.front().aabb);
	for(const ItemInfo& itemInfo : itemInfos)
	{
		nodeAABB = AABB3D::makeUnioned(nodeAABB, itemInfo.aabb);
	}

	if(itemInfos.size() <= 1)
	{
		*node = InfoNode::makeBinaryLeaf(itemInfos, nodeAABB);

#if PH_DEBUG
		if(itemInfos.empty())
		{
			PH_DEFAULT_LOG(Warning,
				"at `BvhBuilder::buildBinaryBvhNodeRecursive()`, leaf node without primitive detected");
		}
#endif
	}
	else
	{
		AABB3D centroidsAABB(itemInfos.front().aabbCentroid);
		for(const ItemInfo& itemInfo : itemInfos)
		{
			centroidsAABB = AABB3D::makeUnioned(centroidsAABB, AABB3D(itemInfo.aabbCentroid));
		}

		Vector3R extents = centroidsAABB.getExtents();
#if PH_DEBUG
		if(!extents.isNonNegative())
		{
			PH_DEFAULT_LOG(Warning,
				"at `BvhBuilder::buildBinaryBvhNodeRecursive()`, negative AABB extent detected");
			extents.absLocal();
		}
#endif

		const auto maxDimension = extents.maxDimension();
		if(centroidsAABB.getMinVertex()[maxDimension] == centroidsAABB.getMaxVertex()[maxDimension])
		{
			*node = InfoNode::makeBinaryLeaf(itemInfos, nodeAABB);
		}
		else
		{
			bool isSplitSuccess = false;
			TSpan<ItemInfo> negativeChildItems;
			TSpan<ItemInfo> positiveChildItems;
			switch(m_splitMethod)
			{

			case EBvhNodeSplitMethod::EqualItems:
				isSplitSuccess = splitWithEqualIntersectables(
					itemInfos,
					maxDimension,
					&negativeChildItems,
					&positiveChildItems);
				break;

			case EBvhNodeSplitMethod::SAH_Buckets:
				isSplitSuccess = splitWithSahBuckets(
					itemInfos,
					maxDimension,
					nodeAABB,
					centroidsAABB,
					&negativeChildItems,
					&positiveChildItems);
				break;

			default:
				PH_DEFAULT_DEBUG_LOG(
					"at `BvhBuilder::buildBinaryBvhNodeRecursive()`, unsupported BVH split method detected");
				isSplitSuccess = false;
				break;

			}// end switch split method

			if(isSplitSuccess && (negativeChildItems.empty() || positiveChildItems.empty()))
			{
				PH_DEFAULT_DEBUG_LOG(
					"at `BvhBuilder::buildBinaryBvhNodeRecursive()`, bad split detected: "
					"#neg-child={}, #pos-child={}", negativeChildItems.size(), positiveChildItems.size());
				isSplitSuccess = false;
			}

			if(isSplitSuccess)
			{
				*node = InfoNode::makeBinaryInternal(
					buildBinaryBvhInfoNodeRecursive<SPLIT_METHOD>(negativeChildItems),
					buildBinaryBvhInfoNodeRecursive<SPLIT_METHOD>(positiveChildItems),
					maxDimension);
			}
			else
			{
				*node = InfoNode::makeBinaryLeaf(itemInfos, nodeAABB);
			}
		}
	}

	return node;
}

template<typename Item, typename ItemToAABB>
template<typename IndexType>
inline void TBvhBuilder<Item, ItemToAABB>
::buildBinaryBvhLinearDepthFirstNodeRecursive(
	const InfoNode* const node,
	TLinearDepthFirstBinaryBvh<Item, IndexType>* const out_bvh)
{
	using LinearNode = TLinearBvhNode<Item>;

	const auto nodeIndex = out_bvh->numNodes;
	const auto itemOffset = out_bvh->numItems;

	if(node->isBinaryLeaf())
	{
		for(std::size_t i = 0; i < node->items.size(); ++i)
		{
			out_bvh->items[itemOffset + i] = node->items[i].item;
		}
		out_bvh->numItems += node->items.size();

		out_bvh->nodes[nodeIndex] = LinearNode::makeLeaf(
			node->aabb,
			itemOffset,
			node->items.size());
		out_bvh->numNodes += 1;
	}
	else if(node->isBinaryInternal())
	{
		LinearNode* linearNode = &(out_bvh->nodes[nodeIndex]);
		out_bvh->numNodes += 1;

		buildBinaryBvhLinearDepthFirstNodeRecursive(node->children[0], out_bvh);

		const auto secondChildOffset = out_bvh->numNodes;
		buildBinaryBvhLinearDepthFirstNodeRecursive(node->children[1], out_bvh);

		*linearNode = LinearNode::makeInternal(
			node->aabb,
			secondChildOffset,
			node->splitAxis);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

template<typename Item, typename ItemToAABB>
inline std::size_t TBvhBuilder<Item, ItemToAABB>
::calcTotalNodes(const InfoNode* const node)
{
	PH_ASSERT(node);

	std::size_t result = 1;
	result += node->children[0] ? calcTotalNodes(node->children[0]) : 0;
	result += node->children[1] ? calcTotalNodes(node->children[1]) : 0;
	return result;
}

template<typename Item, typename ItemToAABB>
inline std::size_t TBvhBuilder<Item, ItemToAABB>
::calcTotalItems(const InfoNode* const node)
{
	PH_ASSERT(node);

	std::size_t result = node->items.size();
	result += node->children[0] ? calcTotalItems(node->children[0]) : 0;
	result += node->children[1] ? calcTotalItems(node->children[1]) : 0;
	return result;
}

template<typename Item, typename ItemToAABB>
inline std::size_t TBvhBuilder<Item, ItemToAABB>
::calcMaxDepth(const InfoNode* const node)
{
	PH_ASSERT(node);

	const std::size_t child1Depth = node->children[0] ? calcMaxDepth(node->children[0]) : 0;
	const std::size_t child2Depth = node->children[1] ? calcMaxDepth(node->children[1]) : 0;

	std::size_t depth = node->children[0] || node->children[1] ? 1 : 0;
	depth += std::max(child1Depth, child2Depth);
	return depth;
}

template<typename Item, typename ItemToAABB>
inline bool TBvhBuilder<Item, ItemToAABB>
::splitWithEqualIntersectables(
	const TSpan<ItemInfo> itemInfos,
	const std::size_t splitDimension,
	TSpan<ItemInfo>* const out_negativePart,
	TSpan<ItemInfo>* const out_positivePart)
{
	if(itemInfos.size() < 2)
	{
		PH_DEFAULT_LOG(Warning,
			"at `BvhBuilder::splitWithEqualPrimitives()`, number of items < 2, cannot split");
		return false;
	}

	const std::size_t midIndex = itemInfos.size() / 2 - 1;

	auto sortedItemInfos = itemInfos;
	std::nth_element(
		sortedItemInfos.begin(),
		sortedItemInfos.begin() + midIndex,
		sortedItemInfos.end(),
		[splitDimension](const ItemInfo& a, const ItemInfo& b)
		{
			return a.aabbCentroid[splitDimension] < b.aabbCentroid[splitDimension];
		});

	PH_ASSERT(out_negativePart);
	PH_ASSERT(out_positivePart);
	*out_negativePart = sortedItemInfos.subspan(0, midIndex + 1);
	*out_positivePart = sortedItemInfos.subspan(midIndex + 1);
	return true;
}

template<typename Item, typename ItemToAABB>
inline bool TBvhBuilder<Item, ItemToAABB>
::splitWithSahBuckets(
	const TSpan<ItemInfo> itemInfos,
	const std::size_t splitDimension,
	const AABB3D& itemsAABB,
	const AABB3D& itemsCentroidAABB,
	TSpan<ItemInfo>* const out_negativePart,
	TSpan<ItemInfo>* const out_positivePart)
{
	if(itemInfos.size() < 2)
	{
		PH_DEFAULT_LOG(Warning,
			"at `BvhBuilder::splitWithSahBuckets()`, number of items < 2, cannot split");
		return false;
	}

	constexpr std::size_t numBuckets = 64;

	const auto dim            = splitDimension;
	const real rcpSplitExtent = safe_rcp(itemsCentroidAABB.getExtents()[dim]);

	PH_ASSERT_GE(rcpSplitExtent, 0.0_r);

	SahBucket buckets[numBuckets];
	for(const ItemInfo& itemInfo : itemInfos)
	{
		const real factor = (itemInfo.aabbCentroid[dim] - itemsCentroidAABB.getMinVertex()[dim]) * rcpSplitExtent;
		auto bucketIndex = static_cast<std::size_t>(factor * numBuckets);
		bucketIndex = (bucketIndex == numBuckets) ? bucketIndex - 1 : bucketIndex;

		buckets[bucketIndex].aabb = buckets[bucketIndex].isEmpty()
			? itemInfo.aabb: buckets[bucketIndex].aabb.unionWith(itemInfo.aabb);
		buckets[bucketIndex].numItems++;
	}

	std::array<real, numBuckets - 1> splitCosts{};
	for(std::size_t i = 0; i < numBuckets - 1; ++i)
	{
		std::size_t numNegPartItems = 0;
		auto negPartAABB = AABB3D::makeEmpty();
		for(std::size_t j = 0; j <= i; ++j)
		{
			numNegPartItems += buckets[j].numItems;
			negPartAABB.unionWith(buckets[j].aabb);
		}

		std::size_t numPosPartItems = 0;
		auto posPartAABB = AABB3D::makeEmpty();
		for(std::size_t j = i + 1; j < numBuckets; ++j)
		{
			numPosPartItems += buckets[j].numItems;
			posPartAABB.unionWith(buckets[j].aabb);
		}

		// Safe clamping probabilities to [0, 1] as the AABBs may be empty, point, plane,
		// or being super large to be Inf/NaN
		const real probTestingNegPart = safe_clamp(
			negPartAABB.getSurfaceArea() / itemsAABB.getSurfaceArea(), 0.0_r, 1.0_r);
		const real probTestingPosPart = safe_clamp(
			posPartAABB.getSurfaceArea() / itemsAABB.getSurfaceArea(), 0.0_r, 1.0_r);

		splitCosts[i] =
			m_params.getTraversalCost() + 
			static_cast<real>(numNegPartItems) * m_params.getInteractCost() * probTestingNegPart +
			static_cast<real>(numPosPartItems) * m_params.getInteractCost() * probTestingPosPart;
	}

	const auto minCostIndex = static_cast<std::size_t>(
		std::min_element(splitCosts.begin(), splitCosts.end()) - splitCosts.begin());
	const real minSplitCost = splitCosts[minCostIndex];
	const real noSplitCost  = m_params.getInteractCost() * static_cast<real>(itemInfos.size());

	const std::size_t maxIntersectables = 256;

	PH_ASSERT(out_negativePart);
	PH_ASSERT(out_positivePart);
	if(minSplitCost < noSplitCost || itemInfos.size() > maxIntersectables)
	{
		auto sortedItemInfos = itemInfos;
		auto posPartBegin = std::partition(
			sortedItemInfos.begin(),
			sortedItemInfos.end(),
			[itemsCentroidAABB, dim, rcpSplitExtent, minCostIndex](const ItemInfo& itemInfo)
			{
				const real factor = (itemInfo.aabbCentroid[dim] - itemsCentroidAABB.getMinVertex()[dim]) * rcpSplitExtent;
				auto bucketIndex = static_cast<std::size_t>(factor * numBuckets);
				bucketIndex = (bucketIndex == numBuckets) ? bucketIndex - 1 : bucketIndex;
				return bucketIndex <= minCostIndex;
			});

		*out_negativePart = sortedItemInfos.subspan(0, posPartBegin - sortedItemInfos.begin());
		*out_positivePart = sortedItemInfos.subspan(posPartBegin - sortedItemInfos.begin());
		return true;
	}
	else
	{
		return false;
	}
}

}// end namespace ph::math
