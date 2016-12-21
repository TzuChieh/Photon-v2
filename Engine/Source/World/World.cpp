#include "World/World.h"
#include "Entity/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Entity/Entity.h"
#include "World/BruteForceIntersector.h"
#include "World/Kdtree/KdtreeIntersector.h"
#include "World/LightSampler/UniformRandomLightSampler.h"

#include <limits>
#include <iostream>

namespace ph
{

World::World() : 
	//m_intersector(std::make_unique<BruteForceIntersector>())
	m_intersector(std::make_unique<KdtreeIntersector>()), 
	m_lightSampler(std::make_unique<UniformRandomLightSampler>())
{

}

void World::addEntity(const Entity& entity)
{
	m_entities.push_back(entity);
}

void World::update(const float32 deltaS)
{
	std::cout << "updating world..." << std::endl;

	gatherPrimitives();

	std::cout << "world discretized into " << m_primitiveStorage.numPrimitives() << " primitives" << std::endl;
	std::cout << "constructing world intersector..." << std::endl;

	m_intersector->update(m_primitiveStorage);
	m_lightSampler->update(m_entities);
}

const Intersector& World::getIntersector() const
{
	return *m_intersector;
}

const LightSampler& World::getLightSampler() const
{
	return *m_lightSampler;
}

void World::gatherPrimitives()
{
	m_primitiveStorage.clear();

	for(const auto& entity : m_entities)
	{
		gatherPrimitivesFromEntity(entity);
	}
}

void World::gatherPrimitivesFromEntity(const Entity& entity)
{
	// a visible entity must at least have geometry and material
	if(entity.getGeometry() && entity.getMaterial())
	{
		auto metadata = std::make_unique<PrimitiveMetadata>();
		metadata->m_material      = entity.getMaterial();
		metadata->m_localToWorld  = entity.getLocalToWorldTransform();
		metadata->m_worldToLocal  = entity.getWorldToLocalTransform();
		metadata->m_textureMapper = entity.getTextureMapper();
		entity.getGeometry()->discretize(&m_primitiveStorage, entity);
		m_primitiveStorage.add(std::move(metadata));
	}

	for(const auto& entity : entity.getChildren())
	{
		gatherPrimitivesFromEntity(entity);
	}
}

}// end namespace ph