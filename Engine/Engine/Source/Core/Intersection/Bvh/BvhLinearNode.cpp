#include "Core/Intersection/Bvh/BvhLinearNode.h"

namespace ph
{

BvhLinearNode BvhLinearNode::makeInternal(const math::AABB3D& nodeAABB,
                                          const std::size_t secondChildOffset, 
                                          const int32 splittedAxis)
{
	BvhLinearNode node;
	node.aabb              = nodeAABB;
	node.secondChildOffset = secondChildOffset;
	node.splittedAxis      = splittedAxis;
	node.m_isLeaf          = false;

	return node;
}

BvhLinearNode BvhLinearNode::makeLeaf(const math::AABB3D& nodeAABB,
                                      const std::size_t primitivesOffset, 
                                      const int32 numPrimitives)
{
	BvhLinearNode node;
	node.aabb             = nodeAABB;
	node.primitivesOffset = primitivesOffset;
	node.numPrimitives    = numPrimitives;
	node.m_isLeaf         = true;

	return node;
}

BvhLinearNode::BvhLinearNode() : 
	aabb(), secondChildOffset(-1), splittedAxis(-1), m_isLeaf(false)
{}

}// end namespace ph
