#include "Core/Intersectable/Intersectable.h"
#include "Core/Bound/AABB3D.h"
#include "Core/Intersection.h"

namespace ph
{

Intersectable::~Intersectable() = default;

bool Intersectable::isIntersecting(const Ray& ray) const
{
	real tempHitT;
	return isIntersecting(ray, &tempHitT);
}

bool Intersectable::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	AABB3D aabb;
	calcAABB(&aabb);
	
	return aabb.isIntersectingVolume(volume);
}

}// end namespace ph