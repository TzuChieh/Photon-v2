#include "Core/Intersectable/Intersector.h"
#include "Core/HitProbe.h"

namespace ph
{

Intersector::~Intersector() = default;

void Intersector::calcIntersectionDetail(const Ray& ray, HitProbe& probe,
                                         HitDetail* const out_detail) const
{
	probe.getCurrentHit()->calcIntersectionDetail(ray, probe, out_detail);
}

}// end namespace ph