#include "World/Intersector/Bvh/BvhInfoNode.h"
#include "Math/Math.h"

#include <iostream>

namespace ph
{

BvhInfoNode BvhInfoNode::makeBinaryInternal(const BvhInfoNode* child1, const BvhInfoNode* child2, const int32 splitAxis)
{
	if(child1 == nullptr || child2 == nullptr)
	{
		std::cerr << "warning: at BvhInfoNode::makeBinaryInternal(), both children cannot be nullptr" << std::endl;
	}

	BvhInfoNode internalNode;
	internalNode.children[0] = child1;
	internalNode.children[1] = child2;
	internalNode.aabb        = AABB::makeUnioned(child1->aabb, child2->aabb);
	internalNode.splitAxis   = splitAxis;

	return internalNode;
}

BvhInfoNode BvhInfoNode::makeBinaryLeaf(const std::vector<BvhPrimitiveInfo>& leafPrimitives, const AABB& leafAabb)
{
	BvhInfoNode leafNode;
	leafNode.aabb       = leafAabb;
	leafNode.primitives = leafPrimitives;

	return leafNode;
}

BvhInfoNode::BvhInfoNode() : 
	children{nullptr, nullptr}, aabb(), splitAxis(Math::UNKNOWN_AXIS), primitives()
{

}

bool BvhInfoNode::isBinaryLeaf() const
{
	return children[0] == nullptr && children[1] == nullptr && !primitives.empty();
}

bool BvhInfoNode::isBinaryInternal() const
{
	return children[0] != nullptr && children[1] != nullptr && primitives.empty();
}

}// end namespace ph