#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/constant.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>
#include <Common/utility.h>
#include <Common/compiler.h>
#include <Common/memory.h>

#include <cstddef>
#include <type_traits>
#include <array>

namespace ph::math
{

/*!
@note The layout of this node is different to ordinary node layout--children are directly stored in
their parent, resulting in a "fat" node.
*/
template<std::size_t N, typename Index>
class TWideBvhNode final
{
	static_assert(N >= 2);
	static_assert(std::is_unsigned_v<Index>);

	inline static constexpr std::size_t NUM_ITEMS_BITS = 5;

	inline static constexpr uint8 X_AXIS_FLAG  = 0b00;
	inline static constexpr uint8 Y_AXIS_FLAG  = 0b01;
	inline static constexpr uint8 Z_AXIS_FLAG  = 0b10;
	inline static constexpr uint8 NO_AXIS_FLAG = 0b11;

	static_assert(constant::X_AXIS == X_AXIS_FLAG);
	static_assert(constant::Y_AXIS == Y_AXIS_FLAG);
	static_assert(constant::Z_AXIS == Z_AXIS_FLAG);

public:
	/*! Maximum number of items in a single node. */
	inline static constexpr std::size_t MAX_NODE_ITEMS = (std::size_t(1) << NUM_ITEMS_BITS) - 1;

	/*! @brief Creates a node that contains `N` empty leaves.
	*/
	TWideBvhNode();

	AABB3D getAABB(std::size_t childIdx) const;
	bool isLeaf(std::size_t childIdx) const;
	bool isInternal(std::size_t childIdx) const;
	std::size_t getChildOffset(std::size_t childIdx) const;

	/*! @brief Obtain min. and max. vertices of the child AABBs in SoA layout.
	@param axis The axis (dimension) to obtain.
	@return View of the coordinates on the specified axis. The view has at least 16-byte alignment.
	*/
	///@{
	TSpanView<real, N> getMinVerticesOnAxis(std::size_t axis) const;
	TSpanView<real, N> getMaxVerticesOnAxis(std::size_t axis) const;
	///@}

	/*!
	The most straightforward interpretation of the split axis is the axis that this child (`childIdx`)
	and next child (`childIdx + 1`) is divided upon. However, some tree types may store the split axis
	in a way such that the axis associated with `childIdx` is for `childIdx + n` and `childIdx + n + 1`,
	where `n >= 0` (and `n` may be different for each child) for efficiency reasons.
	@return The split axis associated with the specified child.
	*/
	std::size_t getSplitAxis(std::size_t childIdx) const;
	
	std::size_t getItemOffset(std::size_t childIdx) const;
	std::size_t numItems(std::size_t childIdx) const;

	TWideBvhNode& setInternal(
		std::size_t childIdx,
		const AABB3D& childAABB,
		std::size_t childOffset,
		std::size_t splitAxis);

	TWideBvhNode& setLeaf(
		std::size_t childIdx,
		const AABB3D& childAABB,
		std::size_t itemOffset,
		std::size_t splitAxis,
		std::size_t numItems);

	TWideBvhNode& setEmptyLeaf(
		std::size_t childIdx,
		std::size_t splitAxis);

private:
	struct ChildData
	{
		uint8 isLeaf : 1 = true;
		uint8 splitAxis : 2 = NO_AXIS_FLAG;
		uint8 numItems : NUM_ITEMS_BITS = 0;
	};

	static_assert(sizeof(ChildData) == sizeof(uint8));

	TAlignedArray<std::array<real, N>, 3, 16> m_aabbMins;
	TAlignedArray<std::array<real, N>, 3, 16> m_aabbMaxs;

	/*!
	For internal nodes, this is the child offset. The first child offset does not need to be stored
	for linear depth first trees. Since this node may contain both internal and leaf nodes, this
	helps to simplify the logics.

	For leaf nodes, this is the item offset. 
	*/
	std::array<Index, N> m_offsets;

	/*!
	Since this is a fat node (children are directly stored in their parent), all child nodes have
	an associated split axis. The axes stored can have multiple interpretations. See `getSplitAxis()`
	for more details.

	For internal nodes, `numItems` is unused.
	*/
	std::array<ChildData, N> m_childrenData;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TWideBvhNode.ipp"
