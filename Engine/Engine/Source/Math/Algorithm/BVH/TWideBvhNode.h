#pragma once

#include "Math/Geometry/TAABB3D.h"

#include <Common/primitive_type.h>
#include <Common/utility.h>

#include <cstddef>
#include <type_traits>
#include <array>

namespace ph::math
{

/*!
@note The layout of this node is different to ordinary node layout--children are directly stored in
their parent, resulting in a "fat" node.
*/
template<std::size_t N, typename Item, typename Index>
class TWideBvhNode final
{
	static_assert(N >= 2);
	static_assert(std::is_unsigned_v<Index>);

	inline static constexpr std::size_t NUM_FLAG_BITS  = 2;
	inline static constexpr std::size_t NUM_ITEMS_BITS = sizeof_in_bits<uint8>() - NUM_FLAG_BITS;

	inline static constexpr uint8 FLAG_BITS_MASK = 0b11;
	inline static constexpr uint8 X_AXIS_FLAG    = 0b00;
	inline static constexpr uint8 Y_AXIS_FLAG    = 0b01;
	inline static constexpr uint8 Z_AXIS_FLAG    = 0b10;
	inline static constexpr uint8 LEAF_FLAG      = 0b11;

public:
	/*! Maximum number of items in a single node. */
	inline static constexpr std::size_t MAX_NODE_ITEMS = (std::size_t(1) << NUM_ITEMS_BITS) - 1;

	/*! @brief Creates a node that contains `N` empty leaves.
	*/
	TWideBvhNode();

	const AABB3D& getAABB(std::size_t childIdx) const;
	bool isLeaf(std::size_t childIdx) const;
	bool isInternal(std::size_t childIdx) const;
	std::size_t getChildOffset(std::size_t childIdx) const;
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
		std::size_t numItems);

private:
	std::array<AABB3D, N> m_aabbs;

	/*!
	For internal nodes, this is the child offset. The first child offset does not need to be stored
	for linear depth first trees. Since this node may contain both internal and leaf nodes, this
	helps to simplify the logics.
	For leaf nodes, this is the item offset. 
	*/
	std::array<Index, N> m_offsets;

	/*!
	We divide `uint8` into two parts: 
	[6 bits][2 bits]. The [2 bits] part has the following meaning

	`X_AXIS_FLAG`: splitting axis is X // indicates this node is internal
	`Y_AXIS_FLAG`: splitting axis is Y //
	`Z_AXIS_FLAG`: splitting axis is Z //
	`LEAF_FLAG`  : this node is leaf

	For internal nodes, the upper [6 bits] is unused.
	For leaf nodes, `numItems` is stored in the upper [6 bits]. 
	*/
	std::array<uint8, N> m_numItemsAndFlags;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TWideBvhNode.ipp"
