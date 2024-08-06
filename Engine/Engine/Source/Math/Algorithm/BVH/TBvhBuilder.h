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

namespace ph::math
{

enum class EBvhNodeSplitMethod
{
	EqualItems,
	Midpoint,
	SAH_Buckets,
	SAH_EdgeSort
};

template<typename Item, typename ItemToAABB>
class TBvhInfoNode;

template<typename Item, typename IndexType>
class TLinearDepthFirstBinaryBvh;

template<typename Item, typename ItemToAABB>
class TBvhBuilder final
{
	static_assert(std::is_invocable_r_v<AABB3D, ItemToAABB, Item>);

public:
	using ItemInfo = TBvhItemInfo<Item, ItemToAABB>;
	using InfoNode = TBvhInfoNode<Item, ItemToAABB>;

	static std::size_t calcTotalNodes(const InfoNode* node);
	static std::size_t calcTotalItems(const InfoNode* node);
	static std::size_t calcMaxDepth(const InfoNode* node);

public:
	explicit TBvhBuilder(
		EBvhNodeSplitMethod splitMethod,
		BvhParams params = BvhParams())
		requires std::default_initializable<ItemToAABB>
		: TBvhBuilder(splitMethod, ItemToAABB{}, params)
	{}

	TBvhBuilder(
		EBvhNodeSplitMethod splitMethod,
		ItemToAABB itemToAABB,
		BvhParams params = BvhParams{});

	auto buildInformativeBinaryBvh(TSpanView<Item> items)
	-> const InfoNode*;

	template<typename IndexType = std::size_t>
	void buildLinearDepthFirstBinaryBvh(
		const InfoNode* rootNode,
		TLinearDepthFirstBinaryBvh<Item, IndexType>* out_bvh);

	void clearBuildData();

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
	auto buildBinaryBvhInfoNodeRecursive(
		TSpan<ItemInfo> itemInfos)
	-> const InfoNode*;

	template<typename IndexType>
	void buildBinaryBvhLinearDepthFirstNodeRecursive(
		const InfoNode* node,
		TLinearDepthFirstBinaryBvh<Item, IndexType>* out_bvh);

	bool splitWithEqualIntersectables(
		TSpan<ItemInfo> itemInfos,
		std::size_t splitDimension,
		TSpan<ItemInfo>* out_negativePart,
		TSpan<ItemInfo>* out_positivePart);

	bool splitWithSahBuckets(
		TSpan<ItemInfo> itemInfos,
		std::size_t splitDimension,
		const AABB3D& itemsAABB,
		const AABB3D& itemsCentroidAABB,
		TSpan<ItemInfo>* out_negativePart,
		TSpan<ItemInfo>* out_positivePart);

	std::vector<ItemInfo> m_infoBuffer;
	std::vector<InfoNode> m_infoNodes;
	BvhParams m_params;
	ItemToAABB m_itemToAABB;
	EBvhNodeSplitMethod m_splitMethod;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhBuilder.ipp"
