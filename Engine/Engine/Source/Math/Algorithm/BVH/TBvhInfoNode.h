#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/constant.h"
#include "Utility/TSpan.h"

#include <cstddef>
#include <array>

namespace ph::math
{

template<typename Item>
class TBvhItemInfo;

/*! @brief General BVH node packed with additional information.
This node type is typically used for building other types of BVH or for algorithmic analysis.
*/
template<std::size_t N, typename Item>
class TBvhInfoNode final
{
	static_assert(N >= 2);

public:
	using ItemInfoType = TBvhItemInfo<Item>;

	static auto makeInternal(
		const std::array<const TBvhInfoNode*, N>& children,
		std::size_t splitAxis) 
	-> TBvhInfoNode;
	
	static auto makeLeaf(
		TSpanView<ItemInfoType> leafItems,
	    const AABB3D& leafAabb)
	-> TBvhInfoNode;

	static constexpr bool isBinary();
	static constexpr std::size_t numChildren();

public:
	TBvhInfoNode();

	bool isBinaryLeaf() const;
	bool isBinaryInternal() const;
	bool isLeaf() const;
	bool isInternal() const;

	auto getChild(std::size_t childIdx) const
	-> const TBvhInfoNode*;

	auto getItems() const
	-> TSpanView<ItemInfoType>;

	auto getAABB() const
	-> const AABB3D&;

	auto getSplitAxis() const
	-> std::size_t;

private:
	std::array<const TBvhInfoNode*, N> m_children;
	TSpanView<ItemInfoType> m_items;
	AABB3D m_aabb;
	uint32f m_splitAxis : 2;
	uint32f m_isLeaf : 1;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhInfoNode.ipp"
