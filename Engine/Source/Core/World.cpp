#include "Core/World.h"
#include "Model/Primitive/Primitive.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"

#include <limits>

namespace ph
{

void World::addPrimitive(const std::shared_ptr<Primitive>& primitive)
{
	m_primitives.push_back(primitive);
}

bool World::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	Intersection intersection;
	float32 closestSquaredHitDist = std::numeric_limits<float32>::infinity();

	for(const auto& primitive : m_primitives)
	{
		if(primitive->isIntersecting(ray, &intersection))
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