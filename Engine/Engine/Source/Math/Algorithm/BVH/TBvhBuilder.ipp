#pragma once

#include "Math/Algorithm/BVH/TBvhBuilder.h"
#include "Math/Algorithm/BVH/TBvhInfoNode.h"
#include "Math/Algorithm/BVH/TBinaryBvhNode.h"
#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.h"
#include "Math/math.h"
#include "Core/Intersection/Intersectable.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <cmath>

namespace ph::math
{

template<std::size_t N, typename Item, typename ItemToAABB>
inline TBvhBuilder<N, Item, ItemToAABB>
::TBvhBuilder(
	ItemToAABB itemToAABB,
	BvhParams params)

	: m_infoBuffer()
	, m_infoNodes()
	, m_params(params)
	, m_itemToAABB(std::move(itemToAABB))
{}

template<std::size_t N, typename Item, typename ItemToAABB>
inline auto TBvhBuilder<N, Item, ItemToAABB>
::buildInformativeBvh(TSpanView<Item> items)
-> const InfoNodeType*
{
	clearBuildData();

	m_infoBuffer.resize(items.size());
	for(std::size_t i = 0; i < items.size(); i++)
	{
		m_infoBuffer[i] = ItemInfoType(m_itemToAABB(items[i]), items[i]);
	}

	// To have stable node pointers, pre-allocate enough nodes beforehand. We calculate the maximum
	// nodes required for a BVH with maximum items in node = 1 and branch factor = 2. This is quite
	// pessimistic for wide BVHs, and a tighter bound is left for future work.
	const std::size_t maxLeaves = items.size();
	const std::size_t maxNodes  = 2 * maxLeaves - 1;
	m_infoNodes.reserve(maxNodes);

	const InfoNodeType* rootNode = nullptr;
	switch(m_params.splitMethod)
	{
	case EBvhNodeSplitMethod::EqualItems:
		rootNode = buildBvhInfoNodeRecursive<EBvhNodeSplitMethod::EqualItems>(
			m_infoBuffer);
		break;

	case EBvhNodeSplitMethod::SAH_Buckets_OneAxis:
		rootNode = buildBvhInfoNodeRecursive<EBvhNodeSplitMethod::SAH_Buckets_OneAxis>(
			m_infoBuffer);
		break;

	default:
		PH_DEFAULT_LOG(Warning,
			"BVH{} builder: unsupported BVH split method", N);
		break;
	}

	PH_ASSERT_EQ(m_infoBuffer.size(), items.size());
	PH_ASSERT_LE(m_infoNodes.size(), maxNodes);

	// Verify the nodes by doing a full traversal
	PH_ASSERT_EQ(m_infoNodes.size(), calcTotalNodes(rootNode));
	PH_ASSERT_EQ(m_infoBuffer.size(), calcTotalItems(rootNode));

	// As noted earlier, we appreciate a tighter bound
	PH_DEFAULT_DEBUG_LOG(
		"BVH{} builder: node buffer utilization = {}",
		N, static_cast<float>(m_infoNodes.size()) / maxNodes);

	return rootNode;
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline void TBvhBuilder<N, Item, ItemToAABB>
::clearBuildData()
{
	m_infoBuffer.clear();
	m_infoNodes.clear();
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline auto TBvhBuilder<N, Item, ItemToAABB>
::totalInfoNodes() const
-> std::size_t
{
	return m_infoNodes.size();
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline auto TBvhBuilder<N, Item, ItemToAABB>
::totalItems() const
-> std::size_t
{
	return m_infoBuffer.size();
}

template<std::size_t N, typename Item, typename ItemToAABB>
template<EBvhNodeSplitMethod SPLIT_METHOD>
inline auto TBvhBuilder<N, Item, ItemToAABB>
::buildBvhInfoNodeRecursive(
	const TSpan<ItemInfoType> itemInfos)
-> const InfoNodeType*
{
	// Creating a new node must not cause reallocation
	PH_ASSERT_LT(m_infoNodes.size(), m_infoNodes.capacity());

	m_infoNodes.push_back(InfoNodeType{});
	InfoNodeType* const node = &m_infoNodes.back();

	AABB3D nodeAABB(itemInfos.empty() ? AABB3D(Vector3R(0)) : itemInfos.front().aabb);
	for(const ItemInfoType& itemInfo : itemInfos)
	{
		nodeAABB = AABB3D::makeUnioned(nodeAABB, itemInfo.aabb);
	}

	// Makes no sense to split
	if(itemInfos.size() <= 1)
	{
		*node = InfoNodeType::makeLeaf(itemInfos, nodeAABB);

#if PH_DEBUG
		if(itemInfos.empty())
		{
			PH_DEFAULT_LOG(Warning,
				"BVH{} builder: leaf node without item detected", N);
		}
#endif
	}
	// Try to split with `SPLIT_METHOD`
	else
	{
		AABB3D centroidsAABB(itemInfos.front().aabbCentroid);
		for(const ItemInfoType& itemInfo : itemInfos)
		{
			centroidsAABB = AABB3D::makeUnioned(centroidsAABB, AABB3D(itemInfo.aabbCentroid));
		}

		Vector3R extents = centroidsAABB.getExtents();
#if PH_DEBUG
		if(!extents.isNonNegative())
		{
			PH_DEFAULT_LOG(Warning,
				"BVH{} builder: negative AABB extent detected", N);
			extents.absLocal();
		}
#endif

		const auto maxDimension = extents.maxDimension();
		if(centroidsAABB.getMinVertex()[maxDimension] == centroidsAABB.getMaxVertex()[maxDimension])
		{
			*node = InfoNodeType::makeLeaf(itemInfos, nodeAABB);
		}
		// Specialized binary node splitting
		else if constexpr(N == 2)
		{
			bool isSplitted = false;
			TSpan<ItemInfoType> negativeChildItems;
			TSpan<ItemInfoType> positiveChildItems;
			if constexpr(SPLIT_METHOD == EBvhNodeSplitMethod::EqualItems)
			{
				isSplitted = binarySplitWithEqualItems(
					itemInfos,
					maxDimension,
					&negativeChildItems,
					&positiveChildItems);
			}
			else if constexpr(SPLIT_METHOD == EBvhNodeSplitMethod::SAH_Buckets_OneAxis)
			{
				isSplitted = binarySplitWithSahBuckets(
					itemInfos,
					maxDimension,
					nodeAABB,
					centroidsAABB,
					&negativeChildItems,
					&positiveChildItems);
			}
			else
			{
				PH_DEFAULT_DEBUG_LOG(
					"BVH{} builder: unsupported BVH split method detected", N);
				isSplitted = false;
			}// end split method

			if(isSplitted && (negativeChildItems.empty() || positiveChildItems.empty()))
			{
				PH_DEFAULT_DEBUG_LOG(
					"BVH{} builder: bad split detected: #neg-child={}, #pos-child={}",
					N, negativeChildItems.size(), positiveChildItems.size());
				isSplitted = false;
			}

			if(isSplitted)
			{
				*node = InfoNodeType::makeInternal(
					{
						buildBvhInfoNodeRecursive<SPLIT_METHOD>(negativeChildItems),
						buildBvhInfoNodeRecursive<SPLIT_METHOD>(positiveChildItems)
					},
					maxDimension);
			}
			else
			{
				*node = InfoNodeType::makeLeaf(itemInfos, nodeAABB);
			}
		}
		// Generalized N-wide node splitting
		else
		{
			bool isSplitted = false;
			std::array<TSpan<ItemInfoType>, N> itemParts;
			if constexpr(SPLIT_METHOD == EBvhNodeSplitMethod::EqualItems)
			{
				isSplitted = splitWithEqualItems(
					itemInfos,
					maxDimension,
					&itemParts);
			}
			else
			{
				PH_DEFAULT_DEBUG_LOG(
					"BVH{} builder: unsupported BVH split method detected", N);
				isSplitted = false;
			}// end split method

			if(isSplitted)
			{
				std::array<const InfoNodeType*, N> children{};
				for(std::size_t ci = 0; ci < N; ++ci)
				{
					if(!itemParts[ci].empty())
					{
						children[ci] = buildBvhInfoNodeRecursive<SPLIT_METHOD>(itemParts[ci]);
					}
				}

				*node = InfoNodeType::makeInternal(
					children,
					maxDimension);
			}
			else
			{
				*node = InfoNodeType::makeLeaf(itemInfos, nodeAABB);
			}
		}
	}

	return node;
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline std::size_t TBvhBuilder<N, Item, ItemToAABB>
::calcTotalNodes(const InfoNodeType* const node)
{
	if(!node)
	{
		return 0;
	}

	std::size_t result = 1;
	for(std::size_t ci = 0; ci < node->numChildren(); ++ci)
	{
		result += node->getChild(ci) ? calcTotalNodes(node->getChild(ci)) : 0;
	}
	return result;
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline std::size_t TBvhBuilder<N, Item, ItemToAABB>
::calcTotalItems(const InfoNodeType* const node)
{
	if(!node)
	{
		return 0;
	}

	std::size_t result = node->getItems().size();
	for(std::size_t ci = 0; ci < node->numChildren(); ++ci)
	{
		result += node->getChild(ci) ? calcTotalItems(node->getChild(ci)) : 0;
	}
	return result;
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline std::size_t TBvhBuilder<N, Item, ItemToAABB>
::calcMaxDepth(const InfoNodeType* const node)
{
	if(!node)
	{
		return 0;
	}

	std::size_t maxDepth = 0;
	for(std::size_t ci = 0; ci < node->numChildren(); ++ci)
	{
		// Only non-empty child can add one more depth
		if(node->getChild(ci))
		{
			maxDepth = std::max(calcMaxDepth(node->getChild(ci)) + 1, maxDepth);
		}
	}
	return maxDepth;
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline bool TBvhBuilder<N, Item, ItemToAABB>
::binarySplitWithEqualItems(
	const TSpan<ItemInfoType> itemInfos,
	const std::size_t splitDimension,
	TSpan<ItemInfoType>* const out_negativePart,
	TSpan<ItemInfoType>* const out_positivePart)
{
	static_assert(N == 2, "Requires a binary BVH builder.");

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
		[splitDimension](const ItemInfoType& a, const ItemInfoType& b)
		{
			return a.aabbCentroid[splitDimension] < b.aabbCentroid[splitDimension];
		});

	PH_ASSERT(out_negativePart);
	PH_ASSERT(out_positivePart);
	*out_negativePart = sortedItemInfos.subspan(0, midIndex + 1);
	*out_positivePart = sortedItemInfos.subspan(midIndex + 1);
	return true;
}

template<std::size_t N, typename Item, typename ItemToAABB>
inline bool TBvhBuilder<N, Item, ItemToAABB>
::binarySplitWithSahBuckets(
	const TSpan<ItemInfoType> itemInfos,
	const std::size_t splitDimension,
	const AABB3D& itemsAABB,
	const AABB3D& itemsCentroidAABB,
	TSpan<ItemInfoType>* const out_negativePart,
	TSpan<ItemInfoType>* const out_positivePart)
{
	static_assert(N == 2, "Requires a binary BVH builder.");

	if(itemInfos.size() < 2)
	{
		PH_DEFAULT_LOG(Warning,
			"at `BvhBuilder::binarySplitWithSahBuckets()`, number of items < 2, cannot split");
		return false;
	}

	constexpr std::size_t numBuckets = 64;

	const auto dim            = splitDimension;
	const real rcpSplitExtent = safe_rcp(itemsCentroidAABB.getExtents()[dim]);

	PH_ASSERT_GE(rcpSplitExtent, 0.0_r);

	SahBucket buckets[numBuckets];
	for(const ItemInfoType& itemInfo : itemInfos)
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
			m_params.traversalCost + 
			static_cast<real>(numNegPartItems) * m_params.interactCost * probTestingNegPart +
			static_cast<real>(numPosPartItems) * m_params.interactCost * probTestingPosPart;
	}

	const auto minCostIndex = static_cast<std::size_t>(
		std::min_element(splitCosts.begin(), splitCosts.end()) - splitCosts.begin());
	const real minSplitCost = splitCosts[minCostIndex];
	const real noSplitCost  = m_params.interactCost * static_cast<real>(itemInfos.size());

	PH_ASSERT(out_negativePart);
	PH_ASSERT(out_positivePart);
	if(minSplitCost < noSplitCost || itemInfos.size() > m_params.maxNodeItems)
	{
		auto sortedItemInfos = itemInfos;
		auto posPartBegin = std::partition(
			sortedItemInfos.begin(),
			sortedItemInfos.end(),
			[itemsCentroidAABB, dim, rcpSplitExtent, minCostIndex](const ItemInfoType& itemInfo)
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

template<std::size_t N, typename Item, typename ItemToAABB>
inline bool TBvhBuilder<N, Item, ItemToAABB>
::splitWithEqualItems(
	TSpan<ItemInfoType> itemInfos,
	const std::size_t splitDimension,
	std::array<TSpan<ItemInfoType>, N>* const out_parts)
{
	PH_ASSERT(out_parts);

	// Partition `itemInfos` into N parts. This is done by a O(n*N/2) method where n is the number of items.
	// Divide and conquer can achieve O(n*logN), but the gain should only be significant for N > 4. We keep
	// this simpler approach for now.
	auto sortedItemInfos = itemInfos;
	for(std::size_t i = 0; i < N; ++i)
	{
		const auto [beginIdx, endIdx] = ith_evenly_divided_range(i, itemInfos.size(), N);

		// No need to rearrange for empty range and the last part
		if(beginIdx < endIdx && i < N - 1)
		{
			std::nth_element(
				sortedItemInfos.begin() + beginIdx,
				sortedItemInfos.begin() + endIdx - 1,// nth, the last element in this interval
				sortedItemInfos.end(),
				[splitDimension](const ItemInfoType& a, const ItemInfoType& b)
				{
					return a.aabbCentroid[splitDimension] < b.aabbCentroid[splitDimension];
				});
		}

		(*out_parts)[i] = sortedItemInfos.subspan(beginIdx, endIdx - beginIdx);
	}
	return true;
}

}// end namespace ph::math
