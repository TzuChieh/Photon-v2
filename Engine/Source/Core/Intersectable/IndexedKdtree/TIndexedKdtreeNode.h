#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>
#include <vector>
#include <limits>
#include <cmath>
#include <climits>

namespace ph
{

/*
	An indexed kD-tree node with compacted memory layout without regarding
	alignment issues.
*/
template<typename Index>
class TIndexedKdtreeNode
{
	// TODO: add constants for max allowed items and index

public:
	static TIndexedKdtreeNode makeInner(
		real                splitPos,
		int                 splitAxisIndex, 
		std::size_t         positiveChildIndex);

	static TIndexedKdtreeNode makeLeaf(
		const Index*        itemIndices,
		std::size_t         numItems,
		std::vector<Index>& indicesBuffer);

	TIndexedKdtreeNode();

	bool isLeaf() const;
	std::size_t positiveChildIndex() const;
	std::size_t numItems() const;
	real splitPos() const;
	int splitAxisIndex() const;
	std::size_t singleItemDirectIndex() const;
	std::size_t itemIndexOffset() const;

private:
	constexpr static std::size_t NUM_U1_NUMBER_BITS = sizeof(Index) * CHAR_BIT - 2;
	constexpr static std::size_t MAX_U1_NUMBER      = (std::size_t(1) << (NUM_U1_NUMBER_BITS - 1)) - 1;

	/*
		For inner nodes: splitting position <splitPos> along the axis of 
		splitting is stored.
		
		For leaf nodes:  the beginning of the indices for accessing item 
		index buffer is stored in <itemIndexOffset> (an exception is when 
		there is only one item, its index is directly stored in <oneItemIndex>).
	*/
	union
	{
		real  u0_splitPos;
		Index u0_oneItemIndex;
		Index u0_itemIndexOffset;
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

template<typename Index>
inline TIndexedKdtreeNode<Index>::TIndexedKdtreeNode() = default;

template<typename Index>
inline TIndexedKdtreeNode<Index> TIndexedKdtreeNode<Index>::makeInner(
	const real        splitPos,
	const int         splitAxisIndex,
	const std::size_t rightChildIndex)
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

template<typename Index>
inline TIndexedKdtreeNode<Index> TIndexedKdtreeNode<Index>::makeLeaf(
	const Index* const  itemIndices,
	const std::size_t   numItems,
	std::vector<Index>& indicesBuffer)
{
	PH_ASSERT(itemIndices && numItems <= MAX_U1_NUMBER);

	TIndexedKdtreeNode node;

	const Index shiftedNumItems = static_cast<Index>(numItems << 2);
	node.u1_flags = 0b11;
	node.u1_numItems |= shiftedNumItems;

	if(numItems == 0)
	{
		node.u0_oneItemIndex = 0;
	}
	else if(numItems == 1)
	{
		node.u0_oneItemIndex = itemIndices[0];
	}
	else
	{
		// For leaf nodes we directly store index offset value in <u0>. If Index
		// is signed type, value conversion from negative Index back to std::size_t 
		// can mess up the stored bits. So here we check that we did not overflow Index.
		// OPT: try to find an efficient way to make use of the sign bit for storing index
		PH_ASSERT(indicesBuffer.size() <= static_cast<std::size_t>(std::numeric_limits<Index>::max()));

		node.u0_itemIndexOffset = static_cast<Index>(indicesBuffer.size());
		for(std::size_t i = 0; i < numItems; ++i)
		{
			indicesBuffer.push_back(itemIndices[i]);
		}
	}

	return node;
}

template<typename Index>
inline bool TIndexedKdtreeNode<Index>::isLeaf() const
{
	return (u1_flags & 0b11) == 0b11;
}

template<typename Index>
inline std::size_t TIndexedKdtreeNode<Index>::positiveChildIndex() const
{
	PH_ASSERT(!isLeaf());

	return static_cast<std::size_t>(u1_positiveChildIndex >> 2);
}

template<typename Index>
inline std::size_t TIndexedKdtreeNode<Index>::numItems() const
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(u1_numItems >> 2);
}

template<typename Index>
inline real TIndexedKdtreeNode<Index>::splitPos() const
{
	PH_ASSERT(!isLeaf());

	return u0_splitPos;
}

template<typename Index>
inline int TIndexedKdtreeNode<Index>::splitAxisIndex() const
{
	PH_ASSERT(!isLeaf());

	return static_cast<int>(u1_flags & 0b11);
}

template<typename Index>
inline std::size_t TIndexedKdtreeNode<Index>::singleItemDirectIndex() const
{
	PH_ASSERT(isLeaf() && numItems() == 1);

	return static_cast<std::size_t>(u0_oneItemIndex);
}

template<typename Index>
inline std::size_t TIndexedKdtreeNode<Index>::itemIndexOffset() const
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(u0_itemIndexOffset);
}

}// end namespace ph