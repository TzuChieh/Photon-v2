#include "Core/Intersectable/Bvh/BvhInfoNode.h"
#include "Math/math.h"

#include <iostream>

namespace ph
{

BvhInfoNode BvhInfoNode::makeBinaryInternal(const BvhInfoNode* child1, 
                                            const BvhInfoNode* child2, 
                                            const int32 splitAxis)
{
	if(child1 == nullptr || child2 == nullptr)
	{
		std::cerr << "warning: at BvhInfoNode::makeBinaryInternal(), " 
		          << "both children cannot be nullptr" << std::endl;
	}

	BvhInfoNode internalNode;
	internalNode.children[0] = child1;
	internalNode.children[1] = child2;
	internalNode.aabb        = AABB3D::makeUnioned(child1->aabb, child2->aabb);
	internalNode.splitAxis   = splitAxis;

	return internalNode;
}

BvhInfoNode BvhInfoNode::makeBinaryLeaf(const std::vector<BvhIntersectableInfo>& leafIntersectables, 
                                        const AABB3D& leafAabb)
{
	BvhInfoNode leafNode;
	leafNode.aabb           = leafAabb;
	leafNode.intersectables = leafIntersectables;

	return leafNode;
}

BvhInfoNode::BvhInfoNode() : 
	children{}, aabb(), splitAxis(static_cast<int32>(math::constant::UNKNOWN_AXIS)), intersectables()
{}

bool BvhInfoNode::isBinaryLeaf() const
{
	return children[0] == nullptr && children[1] == nullptr && !intersectables.empty();
}

bool BvhInfoNode::isBinaryInternal() const
{
	return children[0] != nullptr && children[1] != nullptr && intersectables.empty();
}

}// end namespace ph
