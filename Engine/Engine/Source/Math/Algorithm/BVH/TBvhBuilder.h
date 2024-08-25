#pragma once

#include "Math/Algorithm/BVH/TBvhItemInfo.h"
#include "Math/Algorithm/BVH/BvhParams.h"
#include "Math/constant.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/TVector3.h"
#include "Utility/TSpan.h"

#include <cstddef>
#include <vector>
#include <type_traits>
#include <concepts>
#include <utility>
#include <array>

namespace ph::math
{

template<std::size_t N, typename Item>
class TBvhInfoNode;

template<std::size_t N, typename Item, typename ItemToAABB>
class TBvhBuilder final
{
	static_assert(N > 1);
	static_assert(std::is_invocable_r_v<AABB3D, ItemToAABB, Item>);

	inline static constexpr std::size_t MAX_SAH_BUCKETS = 64;

public:
	using InfoNodeType = TBvhInfoNode<N, Item>;
	using ItemInfoType = TBvhItemInfo<Item>;

	static std::size_t calcTotalNodes(const InfoNodeType* node);
	static std::size_t calcTotalItems(const InfoNodeType* node);
	static std::size_t calcMaxDepth(const InfoNodeType* node);

public:
	explicit TBvhBuilder(
		BvhParams params = BvhParams{})
		requires std::default_initializable<ItemToAABB>
		: TBvhBuilder(ItemToAABB{}, params)
	{}

	explicit TBvhBuilder(
		ItemToAABB itemToAABB,
		BvhParams params = BvhParams{});

	/*! @brief Build a BVH that contains additional information useful for many purposes.
	The built BVH has the following properties:
	- Maximum items: The maximum number of items a leaf node can contain. This value can be
	set by `BvhParams`.
	- Branch factor: The maximum number of children each node can have is `N`.
	- Split axis: The axis of the offset between children. The split axis associated with child `i`
	is for the axis of offset between child `i` and child `i + 1`. 
	- Split method: How the split axis and its offset value is chosen. This can be set by `BvhParams`.
	@return The root node of the built BVH. Memory of the BVH is managed by this builder.
	@note This method will clear any previous build data.
	*/
	auto buildInformativeBvh(TSpanView<Item> items)
	-> const InfoNodeType*;

	void clearBuildData();

	/*!
	@return Number of nodes of the most recent BVH built by this builder.
	*/
	auto totalInfoNodes() const
	-> std::size_t;

	/*!
	@return Number of contained items of the most recent BVH built by this builder.
	*/
	auto totalItems() const
	-> std::size_t;

private:
	struct SahBucket
	{
	public:
		AABB3D      aabb     = AABB3D::makeEmpty();
		std::size_t numItems = 0;

		bool isEmpty() const
		{
			return numItems == 0;
		}
	};

	/*!
	Build and store BVH nodes in depth-first order.
	*/
	template<EBvhNodeSplitMethod SPLIT_METHOD>
	auto buildBvhInfoNodeRecursive(
		TSpan<ItemInfoType> itemInfos)
	-> const InfoNodeType*;

	bool binarySplitWithEqualItems(
		TSpan<ItemInfoType> itemInfos,
		std::size_t splitDimension,
		TSpan<ItemInfoType>* out_negativePart,
		TSpan<ItemInfoType>* out_positivePart);

	bool binarySplitWithSahBuckets(
		TSpan<ItemInfoType> itemInfos,
		std::size_t splitDimension,
		const AABB3D& itemsAABB,
		const AABB3D& itemsCentroidAABB,
		TSpan<ItemInfoType>* out_negativePart,
		TSpan<ItemInfoType>* out_positivePart);

	bool splitWithEqualItems(
		TSpan<ItemInfoType> itemInfos,
		std::size_t splitDimension,
		std::array<TSpan<ItemInfoType>, N>* out_parts);

	bool splitWithSahBuckets(
		TSpan<ItemInfoType> itemInfos,
		std::size_t splitDimension,
		const AABB3D& itemsAABB,
		const AABB3D& itemsCentroidAABB,
		std::array<TSpan<ItemInfoType>, N>* out_parts);

	void splitWithSahBucketsBacktracking(
		std::size_t splitDimension,
		const AABB3D& itemsAABB,
		TSpanView<SahBucket> buckets,
		std::size_t numSplits,
		std::size_t splitBegin,
		const std::array<std::size_t, N>& splitEnds,
		real cost,
		real* out_bestCost,
		std::array<std::size_t, N>* out_bestSplitEnds) const;

	std::vector<ItemInfoType> m_infoBuffer;
	std::vector<InfoNodeType> m_infoNodes;
	BvhParams m_params;
	ItemToAABB m_itemToAABB;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhBuilder.ipp"
