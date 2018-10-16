#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>
#include <vector>
#include <limits>
#include <cmath>

namespace ph
{

template<typename Index>
class TIndexedKdtreeNode
{
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
	real getSplitPos() const;
	int splitAxisIndex() const;
	std::size_t getSingleItemDirectIndex() const;
	std::size_t getItemIndexOffset() const;

private:
	constexpr std::size_t NUM_U1_NUMBER_BITS = sizeof(Index) * CHAR_BIT - 2;
	constexpr std::size_t MAX_U1_NUMBER      = (std::size_t(1) << (NUM_U1_NUMBER_BITS - 1)) - 1;

	union
	{
		real  u0_splitPos;
		Index u0_oneItemIndex;
		Index u0_itemIndexOffset;
	};

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

	node.u1_flags = 0b11;

	const Index shiftedNumItems = static_cast<Index>(numItems << 2);
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
		PH_ASSERT(indicesBuffer.size() <= static_cast<std::size_t>(~Index(0)));

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
inline real TIndexedKdtreeNode<Index>::getSplitPos() const
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
inline std::size_t TIndexedKdtreeNode<Index>::getSingleItemDirectIndex() const
{
	PH_ASSERT(isLeaf() && numItems() == 1);

	return static_cast<std::size_t>(u0_oneItemIndex);
}

template<typename Index>
inline std::size_t TIndexedKdtreeNode<Index>::getItemIndexOffset() const
{
	PH_ASSERT(isLeaf());

	return static_cast<std::size_t>(u0_itemIndexOffset);
}

}// end namespace ph