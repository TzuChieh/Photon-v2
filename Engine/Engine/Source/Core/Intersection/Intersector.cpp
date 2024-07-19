#include "Core/Intersection/Intersector.h"
#include "Core/HitProbe.h"

namespace ph
{

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

}// end namespace ph
