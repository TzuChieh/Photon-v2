#include "World/World.h"
#include "Entity/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Entity/Entity.h"
#include "World/BruteForceIntersector.h"
#include "World/Kdtree/KdtreeIntersector.h"
#include "Entity/Geometry/Triangle.h"

#include <limits>
#include <iostream>

namespace ph
{

World::World() : 
	//m_intersector(std::make_unique<BruteForceIntersector>())
	m_intersector(std::make_unique<KdtreeIntersector>())
{

}

void World::addEntity(const Entity& entity)
{
	m_entities.push_back(entity);
}

void World::update(const float32 deltaS)
{
	std::cout << "updating world..." << std::endl;

	updateIntersector(m_intersector.get(), m_entities);
}

const Intersector& World::getIntersector() const
{
	return *m_intersector;
}

void World::updateIntersector(Intersector* const out_intersector, const std::vector<Entity>& entities)
{
	out_intersector->clearData();

	std::vector<Triangle> triangles;
	for(const auto& entity : entities)
	{
		discretizeModelGeometry(entity, &triangles);
	}

	std::cout << "world discretized into " << triangles.size() << " triangles" << std::endl;
	std::cout << "constructing world intersector..." << std::endl;

	for(const auto& triangle : triangles)
	{
		out_intersector->addTriangle(triangle);
	}

	out_intersector->construct();
}

void World::discretizeModelGeometry(const Entity& entity, std::vector<Triangle>* const out_triangles)
{
	if(entity.getGeometry())
	{
		entity.getGeometry()->discretize(out_triangles, &entity);
	}

	for(const auto& entity : entity.getChildren())
	{
		discretizeModelGeometry(entity, out_triangles);
	}
}

}// end namespace ph