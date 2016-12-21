#pragma once

#include "Common/primitive_type.h"
#include "Entity/Entity.h"
#include "Intersector.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "World/LightSampler/LightSampler.h"
#include "Core/Primitive/PrimitiveStorage.h"

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
	const LightSampler& getLightSampler() const;

private:
	std::vector<Entity> m_entities;
	PrimitiveStorage m_primitiveStorage;

	std::unique_ptr<Intersector> m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;

	void gatherPrimitivesFromEntity(const Entity& entity);
	void gatherPrimitives();
};

}// end namespace ph