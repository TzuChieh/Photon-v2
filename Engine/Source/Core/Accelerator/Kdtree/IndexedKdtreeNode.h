#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>
#include <vector>

namespace ph
{

class IndexedKdtreeNode
{
public:
	static IndexedKdtreeNode makeInner(
		real splitPos,
		int splitAxisIndex, 
		std::size_t rightChildIndex);

	static IndexedKdtreeNode makeLeaf(
		const int* itemIndices,// TODO: templatize
		std::size_t numItems,
		std::vector<int>& indicesBuffer);// TODO: templatize

	IndexedKdtreeNode();

	bool isLeaf() const;
	std::size_t rightChildIndex() const;
	std::size_t numItems() const;
	real getSplitPos() const;
	int splitAxisIndex() const;

private:
	union
	{
		real u0_splitPos;
		int  u0_oneItemIndex;
		int  u0_itemIndexOffset;
	};

	union
	{
		int  u1_flags;
		int  u1_numItems;
		int  u1_rightChildIndex;
	};
};

// In-header Implementations:

inline IndexedKdtreeNode::IndexedKdtreeNode() = default;

inline IndexedKdtreeNode IndexedKdtreeNode::makeInner(
	const real splitPos,
	const int splitAxisIndex,
	const std::size_t rightChildIndex)
{
	IndexedKdtreeNode node;
	node.u0_splitPos = splitPos;

	// TODO: check overflow
	const int shiftedIndex = static_cast<int>(rightChildIndex) << 2;

	node.u1_flags = splitAxisIndex;// TODO: check it's 0, 1, 2
	node.u1_rightChildIndex |= shiftedIndex;

	return node;
}

inline IndexedKdtreeNode IndexedKdtreeNode::makeLeaf(
	const int* const itemIndices,
	const std::size_t numItems,
	std::vector<int>& indicesBuffer)
{
	// TODO: assert null

	IndexedKdtreeNode node;
	node.u1_flags = 0b11;

	// TODO: check overflow
	const int shiftedNumItems = static_cast<int>(numItems) << 2;
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
		node.u0_itemIndexOffset = indicesBuffer.size();
		for(std::size_t i = 0; i < numItems; ++i)
		{
			indicesBuffer.push_back(itemIndices[i]);
		}
	}

	return node;
}

// TODO: assert leaf and inner

inline bool IndexedKdtreeNode::isLeaf() const
{
	return (u1_flags & 0b11) == 0b11;
}

inline std::size_t IndexedKdtreeNode::rightChildIndex() const
{
	return u1_rightChildIndex >> 2;
}

inline std::size_t IndexedKdtreeNode::numItems() const
{
	return u1_numItems >> 2;
}

inline real IndexedKdtreeNode::getSplitPos() const
{
	return u0_splitPos;
}

inline int IndexedKdtreeNode::splitAxisIndex() const
{
	return u1_flags & 0b11;
}

}// end namespace ph