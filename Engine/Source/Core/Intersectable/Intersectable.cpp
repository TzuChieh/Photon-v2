#include "Core/Intersectable/Intersectable.h"
#include "Core/Bound/AABB3D.h"
#include "Core/IntersectionProbe.h"

namespace ph
{

Intersectable::~Intersectable() = default;

bool Intersectable::isIntersecting(const Ray& ray) const
{
	IntersectionProbe dummyProbe;
	return isIntersecting(ray, dummyProbe);
}

bool Intersectable::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	AABB3D aabb;
	calcAABB(&aabb);
	
	return aabb.isIntersectingVolume(volume);
}

}// end namespace ph