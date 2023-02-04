#include "Core/Intersectable/Intersectable.h"
#include "Math/Geometry/TAABB3D.h"
#include "Core/HitProbe.h"

namespace ph
{

bool Intersectable::isOccluding(const Ray& ray) const
{
	HitProbe dummyProbe;
	return isIntersecting(ray, dummyProbe);
}

bool Intersectable::mayOverlapVolume(const math::AABB3D& volume) const
{
	return calcAABB().isIntersectingVolume(volume);
}

}// end namespace ph
