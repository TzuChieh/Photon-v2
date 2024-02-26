#pragma once

#include "Math/constant.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>
#include <limits>
#include <cmath>
#include <climits>
#include <type_traits>

namespace ph::math
{

/*!
An indexed kD-tree node with compacted memory layout without regarding
alignment issues.
*/
template<typename Index, bool USE_SINGLE_ITEM_OPT = true>
class TIndexedKdtreeNode final
{
	// TODO: add constants for max allowed items and index

public:
	static TIndexedKdtreeNode makeInner(
		real                          splitPos,
		math::constant::AxisIndexType splitAxisIndex,
		std::size_t                   positiveChildIndex);

	static TIndexedKdtreeNode makeLeaf(
		Index               index,
		std::size_t         numItems);

	static TIndexedKdtreeNode makeLeaf(
		TSpanView<Index>    itemIndices,
		std::vector<Index>& indexBuffer);

	bool isLeaf() const;
	std::size_t positiveChildIndex() const;
	std::size_t numItems() const;
	real splitPos() const;
	int splitAxisIndex() const;
	std::size_t index() const;
	std::size_t singleItemDirectIndex() const;
	std::size_t indexBufferOffset() const;

private:
	constexpr static std::size_t NUM_U1_NUMBER_BITS = sizeof(Index) * CHAR_BIT - 2;
	constexpr static std::size_t MAX_U1_NUMBER      = (std::size_t(1) << (NUM_U1_NUMBER_BITS - 1)) - 1;

	/*
		For inner nodes: Splitting position <splitPos> along the axis of 
		splitting is stored.
		
		For leaf nodes: An index value for accessing item is stored in <index>.
	*/
	union
	{
		real  u0_splitPos;
		Index u0_index;
	};

	/*
		Assuming Index type has N bits, we divide it into two parts: 
		[N - 2 bits][2 bits]. The [2 bits] part <flags> has the following meaning

		0b00: splitting axis is X // indicates this node is inner
		0b01: splitting axis is Y //
		0b10: splitting axis is Z //
		0b11: this node is leaf

		For inner nodes, <positiveChildIndex> is stored in the upper [N - 2 bits].
		For leaf nodes, <numItems> is stored in the upper [N - 2 bits] instead. 
	*/
	union
	{
		Index u1_flags;
		Index u1_numItems;
		Index u1_positiveChildIndex;
	};
};

// In-header Implementations:

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::makeInner(
		const real                          splitPos,
		const math::constant::AxisIndexType splitAxisIndex,
		const std::size_t                   rightChildIndex) -> TIndexedKdtreeNode
{
	PH_ASSERT(
		(!std::isnan(splitPos) && !std::isinf(splitPos)) &&
		(0 <= splitAxisIndex && splitAxisIndex <= 2)     &&
		(rightChildIndex <= MAX_U1_NUMBER));

	TIndexedKdtreeNode node;

	node.u0_splitPos = splitPos;

	const Index shiftedIndex = static_cast<Index>(rightChildIndex << 2);
	node.u1_flags = static_cast<Index>(splitAxisIndex);
	node.u1_positiveChildIndex |= shiftedIndex;

	return node;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::makeLeaf(
	const Index       index,
	const std::size_t numItems) -> TIndexedKdtreeNode
{
	PH_ASSERT_LE(numItems, MAX_U1_NUMBER);

	TIndexedKdtreeNode node;

	const Index shiftedNumItems = static_cast<Index>(numItems << 2);
	node.u1_flags = 0b11;
	node.u1_numItems |= shiftedNumItems;

	node.u0_index = index;

	return node;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::makeLeaf(
	const TSpanView<Index> itemIndices,
	std::vector<Index>&    indexBuffer) -> TIndexedKdtreeNode
{
	PH_ASSERT(itemIndices.data());
	PH_ASSERT_LE(itemIndices.size(), MAX_U1_NUMBER);

	if(!(USE_SINGLE_ITEM_OPT && itemIndices.size() == 1))
	{
		// For leaf nodes we directly store index offset value in <u0>. If Index is signed type, 
		// value conversion from negative Index back to std::size_t can mess up the stored bits. 
		// So here we make sure that we will not overflow Index.
		// OPT: try to find an efficient way to make use of the sign bit for storing index
		PH_ASSERT(indexBuffer.size() <= static_cast<std::size_t>(std::numeric_limits<Index>::max()));
		const Index indexBufferOffset = static_cast<Index>(indexBuffer.size());

		indexBuffer.insert(indexBuffer.end(), itemIndices.begin(), itemIndices.end());

		return makeLeaf(indexBufferOffset, itemIndices.size());
	}
	else
	{
		return makeLeaf(itemIndices[0], itemIndices.size());
	}
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::isLeaf() const -> bool
{
	return (u1_flags & 0b11) == 0b11;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::positiveChildIndex() const -> std::size_t
{
	PH_ASSERT(!isLeaf());

	return static_cast<std::size_t>(u1_positiveChildIndex >> 2);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::numItems() const -> std::size_t
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(u1_numItems >> 2);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::splitPos() const -> real
{
	PH_ASSERT(!isLeaf());

	return u0_splitPos;
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::splitAxisIndex() const -> int
{
	PH_ASSERT(!isLeaf());

	return static_cast<int>(u1_flags & 0b11);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::index() const -> std::size_t
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(u0_index);
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::singleItemDirectIndex() const -> std::size_t
{
	if constexpr(USE_SINGLE_ITEM_OPT)
	{
		PH_ASSERT(USE_SINGLE_ITEM_OPT && numItems() == 1);

		return index();
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return std::size_t(-1);
	}
}

template<typename Index, bool USE_SINGLE_ITEM_OPT>
inline auto TIndexedKdtreeNode<Index, USE_SINGLE_ITEM_OPT>
::indexBufferOffset() const -> std::size_t
{
	PH_ASSERT(!(USE_SINGLE_ITEM_OPT && numItems() == 1));

	return index();
}

}// end namespace ph::math
