#include "Core/Intersection/Bvh/BvhIntersectableInfo.h"
#include "Core/Intersection/Intersectable.h"

#include <iostream>

namespace ph
{

BvhIntersectableInfo::BvhIntersectableInfo(
	const Intersectable* const intersectable, 
	const std::size_t          index) :
	index(index), intersectable(intersectable)
{
	aabb         = intersectable->calcAABB();
	aabbCentroid = aabb.getCentroid();
}

}// end namespace ph
