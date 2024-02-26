#include "Core/Intersection/Intersector.h"
#include "Core/HitProbe.h"

namespace ph
{

void Intersector::calcHitDetail(
	const Ray&       ray, 
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	probe.getCurrentHit()->calcHitDetail(ray, probe, out_detail);
}

}// end namespace ph
