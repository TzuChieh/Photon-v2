#include "World/World.h"
#include "Entity/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Entity/Entity.h"
#include "World/BruteForceIntersector.h"
#include "World/Kdtree/KdtreeIntersector.h"

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

	updateIntersector(m_intersector.get(), m_entities, &m_primitives);
}

const Intersector& World::getIntersector() const
{
	return *m_intersector;
}

const LightStorage& World::getLightStorage() const
{
	return m_lightStorage;
}

void World::updateIntersector(Intersector* const out_intersector, const std::vector<Entity>& entities, std::vector<std::unique_ptr<Primitive>>* const out_primitives)
{
	out_primitives->clear();
	out_primitives->shrink_to_fit();

	for(const auto& entity : entities)
	{
		discretizeEntity(entity, out_primitives);
	}

	std::cout << "world discretized into " << out_primitives->size() << " primitives" << std::endl;
	std::cout << "constructing world intersector..." << std::endl;

	out_intersector->update(*out_primitives);
}

void World::discretizeEntity(const Entity& entity, std::vector<std::unique_ptr<Primitive>>* const out_primitives)
{
	// a visible entity must at least have geometry and material
	if(entity.getGeometry() && entity.getMaterial())
	{
		auto metadata = std::make_unique<PrimitiveMetadata>();
		metadata->m_material      = entity.getMaterial();
		metadata->m_localToWorld  = entity.getLocalToWorldTransform();
		metadata->m_worldToLocal  = entity.getWorldToLocalTransform();
		metadata->m_textureMapper = entity.getTextureMapper();
		m_primitiveMetadataBuffer.push_back(std::move(metadata));
		entity.getGeometry()->discretize(out_primitives, m_primitiveMetadataBuffer.back().get());
	}

	for(const auto& entity : entity.getChildren())
	{
		discretizeEntity(entity, out_primitives);
	}
}

}// end namespace ph