#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/HitEvent.h"
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

bool Intersector::reintersect(HitEvent& event, HitEvent& srcEvent) const
{
	return srcEvent.getTopHit()->reintersect(event, srcEvent);
}

void Intersector::calcHitDetail(HitEvent& event, HitDetail* const out_detail) const
{
	event.getTopHit()->calcHitDetail(event, out_detail);
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
	HitEvent event(remaining);
	while(remaining.getMinT() < remaining.getMaxT() &&
	      target.isIntersecting(event))
	{
		const real hitT = event.getHitRayT();
		if(remaining.getMinT() <= hitT && hitT <= remaining.getMaxT())
		{
			visitor(event);
		}
		// Advance past the hit for, e.g., closed shapes
		remaining.setMinT(std::nextafter(
			std::max(hitT, remaining.getMinT()), remaining.getMaxT()));
		event.reset(remaining);
	}
}

}// end namespace ph
