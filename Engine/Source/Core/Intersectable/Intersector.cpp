#include "Core/Intersectable/Intersector.h"
#include "Core/IntersectionProbe.h"

namespace ph
{

Intersector::~Intersector() = default;

void Intersector::calcIntersectionDetail(const Ray& ray, const IntersectionProbe& probe,
                                         IntersectionDetail* const out_detail) const
{
	probe.hitTarget->calcIntersectionDetail(ray, probe, out_detail);
}

}// end namespace ph