#include "World/KdtreeIntersector.h"

namespace ph
{

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::construct(const std::vector<std::unique_ptr<Primitive>>& primitives)
{

}

bool KdtreeIntersector::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	return false;
}

}// end namespace ph