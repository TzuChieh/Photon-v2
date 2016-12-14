#include "World/BruteForceIntersector.h"
#include "Common/primitive_type.h"
#include "Core/Intersection.h"
#include "Core/Ray.h"
#include "Entity/Geometry/Triangle.h"

#include <limits>

namespace ph
{

BruteForceIntersector::~BruteForceIntersector() = default;

void BruteForceIntersector::construct()
{
	// no need to construct anything
}

bool BruteForceIntersector::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	Intersection intersection;
	float32 closestSquaredHitDist = std::numeric_limits<float32>::infinity();

	for(const auto& triangle : m_triangles)
	{
		if(triangle.isIntersecting(ray, &intersection))
		{
			float32 squaredHitDist = intersection.getHitPosition().sub(ray.getOrigin()).squaredLength();
			if(closestSquaredHitDist > squaredHitDist)
			{
				closestSquaredHitDist = squaredHitDist;
				*out_intersection = intersection;
			}
		}
	}

	return closestSquaredHitDist != std::numeric_limits<float32>::infinity();
}

}// end namespace ph