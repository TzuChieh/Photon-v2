#pragma once

#include "Math/constant.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/utility.h>

#include <cstddef>
#include <vector>
#include <limits>
#include <cmath>
#include <type_traits>

namespace ph::math
{

/*! @brief An indexed kD-tree node with compacted memory layout.
*/
template<typename Index, bool USE_SINGLE_ITEM_OPT = true>
class TIndexedKdtreeNode final
{
	// Theoretically we can use a signed type. Limiting to unsigned types to simplify the implementation.
	static_assert(std::is_unsigned_v<Index>);

	inline static constexpr std::size_t NUM_FLAG_BITS   = 2;
	inline static constexpr std::size_t NUM_NUMBER_BITS = sizeof_in_bits<Index>() - NUM_FLAG_BITS;
	inline static constexpr std::size_t MAX_NUMBER      = (std::size_t(1) << NUM_NUMBER_BITS) - 1;

	inline static constexpr Index FLAG_BITS_MASK = 0b11;
	inline static constexpr Index X_AXIS_FLAG    = 0b00;
	inline static constexpr Index Y_AXIS_FLAG    = 0b01;
	inline static constexpr Index Z_AXIS_FLAG    = 0b10;
	inline static constexpr Index LEAF_FLAG      = 0b11;

public:
	/*! Maximum number of items in a single node. */
	inline static constexpr std::size_t MAX_NODE_ITEMS = MAX_NUMBER;

	/*! Maximum node index. */
	inline static constexpr std::size_t MAX_NODE_INDEX = MAX_NUMBER;

	/*! Maximum offset for the item buffer or the index buffer (depending on `USE_SINGLE_ITEM_OPT`).
	Note that the maximum item index can still be larger than this value, as it is computed from
	the offset and number of items. */
	inline static constexpr std::size_t MAX_BUFFER_OFFSET = std::numeric_limits<Index>::max();

	static TIndexedKdtreeNode makeInner(
		real                splitPos,
		std::size_t         splitAxisIndex,
		std::size_t         positiveChildIndex);

	static TIndexedKdtreeNode makeLeaf(
		Index               indexBufferOffset,
		std::size_t         numItems);

	static TIndexedKdtreeNode makeLeaf(
		TSpanView<Index>    itemIndices,
		std::vector<Index>& indexBuffer);

	bool isLeaf() const;
	std::size_t getPositiveChildIndex() const;
	std::size_t numItems() const;
	real getSplitPos() const;
	std::size_t getSplitAxis() const;
	std::size_t getSingleItemDirectIndex() const;
	std::size_t getIndexBufferOffset() const;

private:
	/*!
	For inner nodes: Splitting position `splitPos` along the axis of 
	splitting is stored.
		
	For leaf nodes: An offset into the index buffer is stored in `indexBufferOffset`. If
	`USE_SINGLE_ITEM_OPT` is set and number of items in the node is 1, an offset into the item buffer
	is stored directly in `itemBufferOffset`.
	*/
	union
	{
		real  u0_splitPos;
		Index u0_indexBufferOffset;
		Index u0_itemBufferOffset;
	};

	/*!
	Assuming `Index` has N bits, we divide it into two parts: 
	[N - 2 bits][2 bits]. The [2 bits] part has the following meaning

	`X_AXIS_FLAG`: splitting axis is X // indicates this node is inner
	`Y_AXIS_FLAG`: splitting axis is Y //
	`Z_AXIS_FLAG`: splitting axis is Z //
	`LEAF_FLAG`  : this node is leaf

	For inner nodes, `positiveChildIndex` is stored in the upper [N - 2 bits].
	For leaf nodes, `numItems` is stored in the upper [N - 2 bits]. 
	*/
	Index m_numberAndFlags;
};

// In-header Implementations:

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::makeInner(
	const real        splitPos,
	const std::size_t splitAxisIndex,
	const std::size_t positiveChildIndex)
-> TIndexedKdtreeNode
{
	static_assert(constant::X_AXIS == X_AXIS_FLAG);
	static_assert(constant::Y_AXIS == Y_AXIS_FLAG);
	static_assert(constant::Z_AXIS == Z_AXIS_FLAG);

	PH_ASSERT(!std::isnan(splitPos) && !std::isinf(splitPos));
	PH_ASSERT_IN_RANGE_INCLUSIVE(splitAxisIndex, 0, 2);
	PH_ASSERT_LE(positiveChildIndex, MAX_NODE_INDEX);

	TIndexedKdtreeNode node;
	node.u0_splitPos = splitPos;
	node.m_numberAndFlags = static_cast<Index>((positiveChildIndex << NUM_FLAG_BITS) | splitAxisIndex);

	return node;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::makeLeaf(
	const Index       indexBufferOffset,
	const std::size_t numItems)
-> TIndexedKdtreeNode
{
	PH_ASSERT_LE(numItems, MAX_NODE_ITEMS);

	TIndexedKdtreeNode node;
	node.m_numberAndFlags     = static_cast<Index>((numItems << NUM_FLAG_BITS) | LEAF_FLAG);
	node.u0_indexBufferOffset = lossless_cast<decltype(node.u0_indexBufferOffset)>(indexBufferOffset);

	return node;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::makeLeaf(
	const TSpanView<Index> itemIndices,
	std::vector<Index>&    indexBuffer)
-> TIndexedKdtreeNode
{
	PH_ASSERT(itemIndices.data());

	// General case: indirect item access via index buffer
	if(!(USE_SINGLE_ITEM_OPT && itemIndices.size() == 1))
	{
		// For leaf nodes we directly store index buffer offset in `u0`. Make sure that it
		// will not overflow `Index`.
		PH_ASSERT_LE(indexBuffer.size(), MAX_BUFFER_OFFSET);
		const Index indexBufferOffset = static_cast<Index>(indexBuffer.size());

		indexBuffer.insert(indexBuffer.end(), itemIndices.begin(), itemIndices.end());

		return makeLeaf(indexBufferOffset, itemIndices.size());
	}
	// Special case: direct item access
	else
	{
		// Make sure item buffer offset will no overflow `Index`.
		PH_ASSERT_LE(itemIndices[0], MAX_BUFFER_OFFSET);

		constexpr Index oneItemAndLeafFlag = static_cast<Index>((1 << NUM_FLAG_BITS) | LEAF_FLAG);

		TIndexedKdtreeNode node;
		node.m_numberAndFlags    = oneItemAndLeafFlag;
		node.u0_itemBufferOffset = itemIndices[0];

		return node;
	}
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::isLeaf() const
-> bool
{
	return (m_numberAndFlags & FLAG_BITS_MASK) == LEAF_FLAG;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::getPositiveChildIndex() const
-> std::size_t
{
	PH_ASSERT(!isLeaf());

	return static_cast<std::size_t>(m_numberAndFlags >> NUM_FLAG_BITS);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::numItems() const
-> std::size_t
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(m_numberAndFlags >> NUM_FLAG_BITS);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::getSplitPos() const
-> real
{
	PH_ASSERT(!isLeaf());

	return u0_splitPos;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::getSplitAxis() const
-> std::size_t
{
	PH_ASSERT(!isLeaf());

	return static_cast<std::size_t>(m_numberAndFlags & FLAG_BITS_MASK);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::getSingleItemDirectIndex() const
-> std::size_t
{
	PH_ASSERT(isLeaf());
	if constexpr(USE_SINGLE_ITEM_OPT)
	{
		PH_ASSERT(USE_SINGLE_ITEM_OPT && numItems() == 1);

		return u0_itemBufferOffset;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return static_cast<std::size_t>(-1);
	}
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::getIndexBufferOffset() const
-> std::size_t
{
	PH_ASSERT(isLeaf());
	PH_ASSERT(!(USE_SINGLE_ITEM_OPT && numItems() == 1));

	return u0_indexBufferOffset;
}

}// end namespace ph::math
