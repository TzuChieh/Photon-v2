#include "World/World.h"
#include "Model/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Model/Model.h"
#include "Model/Primitive/Primitive.h"
#include "World/BruteForceIntersector.h"
#include "World/KdtreeIntersector.h"

#include <limits>
#include <iostream>

namespace ph
{

World::World() : 
	//m_intersector(std::make_unique<BruteForceIntersector>())
	m_intersector(std::make_unique<KdtreeIntersector>())
{

}

void World::addModel(const Model& model)
{
	m_models.push_back(std::make_unique<Model>(model));
}

bool World::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	return m_intersector->isIntersecting(ray, out_intersection);
}

void World::cook()
{
	m_primitives.clear();
	m_primitives.shrink_to_fit();

	for(const auto& model : m_models)
	{
		model->getGeometry()->genPrimitives(&m_primitives, model.get());
	}

	m_intersector->construct(m_primitives);
}

}// end namespace ph