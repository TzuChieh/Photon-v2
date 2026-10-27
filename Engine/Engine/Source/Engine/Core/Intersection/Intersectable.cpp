#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Core/HitEvent.h"

namespace ph
{

bool Intersectable::isOccluding(const Ray& ray) const
{
	HitEvent tempEvent(ray);
	return isIntersecting(tempEvent);
}

bool Intersectable::mayOverlapVolume(const math::AABB3D& volume) const
{
	return calcAABB().isIntersectingVolume(volume);
}

}// end namespace ph
