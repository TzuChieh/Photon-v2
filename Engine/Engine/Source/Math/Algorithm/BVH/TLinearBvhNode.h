#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Utility/utility.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <type_traits>

namespace ph::math
{

template<typename Item, typename Index>
class TLinearBvhNode final
{
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

	static auto makeInternal(
		const AABB3D& nodeAABB,
		std::size_t secondChildOffset,
		std::size_t splitAxis)
	-> TLinearBvhNode;

	static auto makeLeaf(
		const AABB3D& nodeAABB,
		std::size_t itemOffset,
		std::size_t numItems)
	-> TLinearBvhNode;

	TLinearBvhNode();

	const AABB3D& getAABB() const;
	bool isLeaf() const;
	bool isInternal() const;
	std::size_t getSecondChildOffset() const;
	std::size_t getSplitAxis() const;
	std::size_t getItemOffset() const;
	std::size_t numItems() const;

private:
	AABB3D m_aabb;

	union
	{
		Index u0_secondChildOffset;// for internal
		Index u0_itemOffset;       // for leaf
	};

	/*!
	Assuming `Index` has N bits, we divide it into two parts: 
	[N - 2 bits][2 bits]. The [2 bits] part has the following meaning

	`X_AXIS_FLAG`: splitting axis is X // indicates this node is internal
	`Y_AXIS_FLAG`: splitting axis is Y //
	`Z_AXIS_FLAG`: splitting axis is Z //
	`LEAF_FLAG`  : this node is leaf

	For internal nodes, the upper [N - 2 bits] is unused.
	For leaf nodes, `numItems` is stored in the upper [N - 2 bits]. 
	*/
	uint8 m_numItemsAndFlags;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearBvhNode.ipp"
