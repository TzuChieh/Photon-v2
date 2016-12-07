#include "World/World.h"
#include "Model/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Model/Model.h"
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

void World::update(const float32 deltaS)
{
	std::cout << "updating world..." << std::endl;

	updateIntersector(m_intersector.get(), m_models);
}

const Intersector& World::getIntersector() const
{
	return *m_intersector;
}

void World::updateIntersector(Intersector* const out_intersector, const std::vector<Model>& models)
{
	out_intersector->clearData();

	std::vector<Triangle> triangles;
	for(const auto& model : models)
	{
		discretizeModelGeometry(model, &triangles);
	}

	std::cout << "world discretized into " << triangles.size() << " triangles" << std::endl;
	std::cout << "constructing world intersector..." << std::endl;

	for(const auto& triangle : triangles)
	{
		out_intersector->addTriangle(triangle);
	}

	out_intersector->construct();
}

void World::discretizeModelGeometry(const Model& model, std::vector<Triangle>* const out_triangles)
{
	if(model.getGeometry())
	{
		model.getGeometry()->discretize(out_triangles, &model);
	}

	for(const auto& model : model.getChildren())
	{
		discretizeModelGeometry(model, out_triangles);
	}
}

}// end namespace ph