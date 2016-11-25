#include "World/World.h"
#include "Model/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Model/Model.h"
#include "Model/Primitive/Primitive.h"
#include "World/BruteForceIntersector.h"
#include "World/KdtreeIntersector.h"
#include "Model/Geometry/Triangle.h"

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
	if(model.getGeometry() == nullptr || model.getMaterial() == nullptr)
	{
		std::cerr << "warning: at World::addModel(), model has no geometry or material" << std::endl;
		return;
	}

	m_models.push_back(model);
}

bool World::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	return m_intersector->isIntersecting(ray, out_intersection);
}

void World::cook()
{
	m_triangles.clear();
	m_triangles.shrink_to_fit();

	for(const auto& model : m_models)
	{
		model.getGeometry()->discretize(&m_triangles, &model);
	}

	m_intersector->construct(m_triangles);
}

}// end namespace ph