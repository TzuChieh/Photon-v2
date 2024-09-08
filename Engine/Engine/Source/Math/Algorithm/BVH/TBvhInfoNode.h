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
		std::size_t singleSplitAxis) 
	-> TBvhInfoNode;
	
	static auto makeLeaf(
		TSpanView<ItemInfoType> leafItems,
	    const AABB3D& leafAabb)
	-> TBvhInfoNode;

	static constexpr bool isBinary();
	static constexpr std::size_t numChildren();
	static constexpr std::size_t noAxisFlag();

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

	auto getSingleSplitAxis() const
	-> std::size_t;

	/*!
	@return The split axis associated with the specified child.
	*/
	auto getSplitAxis(std::size_t childIdx) const
	-> std::size_t;

	bool isSingleSplitAxis() const;

private:
	inline static constexpr uint8 NO_AXIS_FLAG = 0b11;

	static_assert(
		constant::X_AXIS < NO_AXIS_FLAG &&
		constant::Y_AXIS < NO_AXIS_FLAG &&
		constant::Z_AXIS < NO_AXIS_FLAG);

	struct ChildFlags
	{
		uint8 splitAxis : 2 = NO_AXIS_FLAG;
	};

	std::array<const TBvhInfoNode*, N> m_children;
	TSpanView<ItemInfoType> m_items;
	AABB3D m_aabb;

	/*! Flags for each child node. */
	std::array<ChildFlags, N> m_perChildFlags;

	/*! If set to an axis, means all children are split on that axis. */
	uint32f m_singleSplitAxis : 2;

	uint32f m_isLeaf : 1;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhInfoNode.ipp"
