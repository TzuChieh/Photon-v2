#pragma once

#include "Common/primitive_type.h"
#include "Entity/Entity.h"
#include "Intersector.h"
#include "Entity/Primitive/Primitive.h"

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

	const Intersector& getIntersector() const;

private:
	std::vector<Entity> m_entities;

	std::unique_ptr<Intersector> m_intersector;
	std::vector<std::unique_ptr<Primitive>> m_primitives;

	static void updateIntersector(Intersector* const out_intersector, const std::vector<Entity>& entities, std::vector<std::unique_ptr<Primitive>>* const out_primitives);
	static void discretizeEntity(const Entity& entity, std::vector<std::unique_ptr<Primitive>>* const out_primitives);
};

}// end namespace ph