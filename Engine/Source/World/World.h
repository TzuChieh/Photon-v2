#pragma once

#include "Common/primitive_type.h"
#include "Entity/Entity.h"
#include "Intersector.h"
#include "Entity/Primitive/Primitive.h"
#include "World/LightStorage.h"
#include "Entity/Primitive/PrimitiveMetadata.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersection;
class Ray;

class World final
{
public:
	World();

	void addEntity(const Entity& entity);
	void update(const float32 deltaS);

	const Intersector&  getIntersector() const;
	const LightStorage& getLightStorage() const;

private:
	std::vector<Entity> m_entities;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_primitiveMetadataBuffer;

	std::unique_ptr<Intersector> m_intersector;
	LightStorage m_lightStorage;
	std::vector<std::unique_ptr<Primitive>> m_primitives;

	void updateIntersector(Intersector* const out_intersector, const std::vector<Entity>& entities, std::vector<std::unique_ptr<Primitive>>* const out_primitives);
	void discretizeEntity(const Entity& entity, std::vector<std::unique_ptr<Primitive>>* const out_primitives);
};

}// end namespace ph