#include "Core/World.h"
#include "Model/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Model/Model.h"
#include "Model/Geometry/Primitive.h"

#include <limits>

namespace ph
{

void World::addModel(const Model& model)
{
	m_models.push_back(model);
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

void World::cook()
{
	m_primitives.clear();
	m_primitives.shrink_to_fit();

	for(const auto& model : m_models)
	{
		model.getGeometry()->genPrimitives(&m_primitives);
	}
}

}// end namespace ph