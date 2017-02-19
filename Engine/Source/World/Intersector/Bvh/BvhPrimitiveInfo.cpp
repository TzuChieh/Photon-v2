#include "World/Intersector/Bvh/BvhPrimitiveInfo.h"
#include "Core/Primitive/Primitive.h"

#include <iostream>

namespace ph
{

BvhPrimitiveInfo::BvhPrimitiveInfo(const Primitive* primitive, const std::size_t index) :
	index(index), aabb(), aabbCentroid(), primitive(primitive)
{
	primitive->calcAABB(&aabb);
	aabbCentroid = aabb.calcCentroid();
}

}// end namespace ph