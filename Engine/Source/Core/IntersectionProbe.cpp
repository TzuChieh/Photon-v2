#include "Core/IntersectionProbe.h"
#include "Core/IntersectionDetail.h"
#include "Core/Intersectable/Intersectable.h"

namespace ph
{

void IntersectionProbe::calcIntersectionDetail(const Ray& ray, 
                                               IntersectionDetail* const out_detail) const
{
	hitTarget->calcIntersectionDetail(ray, *this, out_detail);
}

}// end namespace ph