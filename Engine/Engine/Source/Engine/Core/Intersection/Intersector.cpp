#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/Ray.h"

#include <Common/exceptions.h>

#include <algorithm>
#include <cmath>

namespace ph
{

void Intersector::forEachIntersection(const Ray& /* ray */, const HitVisitor& /* visitor */) const
{
	throw IllegalOperationException("intersection enumeration is unsupported by this intersector");
}

bool Intersector::reintersect(
	const Ray& ray,
	HitProbe& probe,
	const Ray& srcRay,
	HitProbe& srcProbe) const
{
	return srcProbe.getTopHit()->reintersect(ray, probe, srcRay, srcProbe);
}

void Intersector::calcHitDetail(
	const Ray&       ray, 
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	probe.getTopHit()->calcHitDetail(ray, probe, out_detail);
}

bool Intersector::supportsForEachIntersection() const
{
	return false;
}

void Intersector::forEachIntersectionInTarget(
	const Intersectable& target,
	const Ray& ray,
	const HitVisitor& visitor)
{
	Ray remaining(ray);
	HitProbe probe{};
	while(remaining.getMinT() < remaining.getMaxT() &&
	      target.isIntersecting(remaining, probe))
	{
		const real hitT = probe.getHitRayT();
		if(remaining.getMinT() <= hitT && hitT <= remaining.getMaxT())
		{
			visitor(remaining, probe);
		}
		// Advance past the hit for, e.g., closed shapes
		remaining.setMinT(std::nextafter(
			std::max(hitT, remaining.getMinT()), remaining.getMaxT()));
		probe = HitProbe{};
	}
}

}// end namespace ph
