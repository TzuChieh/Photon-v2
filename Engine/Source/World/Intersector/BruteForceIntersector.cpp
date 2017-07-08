#include "World/Intersector/BruteForceIntersector.h"
#include "Common/primitive_type.h"
#include "Core/Intersection.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/CookedActorStorage.h"

#include <limits>

namespace ph
{

BruteForceIntersector::~BruteForceIntersector() = default;

void BruteForceIntersector::update(const CookedActorStorage& cookedActors)
{
	m_primitives.clear();
	m_primitives.shrink_to_fit();

	for(const auto& primitive : cookedActors.primitives())
	{
		m_primitives.push_back(primitive.get());
	}
}

bool BruteForceIntersector::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	Intersection intersection;
	real closestSquaredHitDist = std::numeric_limits<real>::infinity();

	for(const Primitive* primitive : m_primitives)
	{
		if(primitive->isIntersecting(ray, &intersection))
		{
			real squaredHitDist = intersection.getHitPosition().sub(ray.getOrigin()).lengthSquared();
			if(closestSquaredHitDist > squaredHitDist)
			{
				closestSquaredHitDist = squaredHitDist;
				*out_intersection = intersection;
			}
		}
	}

	return closestSquaredHitDist != std::numeric_limits<real>::infinity();
}

bool BruteForceIntersector::isIntersecting(const Ray& ray) const
{
	for(const Primitive* primitive : m_primitives)
	{
		if(primitive->isIntersecting(ray))
		{
			return true;
		}
	}

	return false;
}

}// end namespace ph