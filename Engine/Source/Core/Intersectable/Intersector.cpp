#include "Core/Intersectable/Intersector.h"
#include "Core/IntersectionProbe.h"

namespace ph
{

Intersector::~Intersector() = default;

void Intersector::calcIntersectionDetail(const Ray& ray, IntersectionProbe& probe,
                                         IntersectionDetail* const out_detail) const
{
	probe.getCurrentHit()->calcIntersectionDetail(ray, probe, out_detail);
}

}// end namespace ph