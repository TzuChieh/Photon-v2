#pragma once

#include "Common/primitive_type.h"
#include "Entity/Geometry/Triangle.h"
#include "Entity/Entity.h"
#include "Intersector.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersection;
class Ray;
class Triangle;

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

	static void updateIntersector(Intersector* const out_intersector, const std::vector<Entity>& entities);
	static void discretizeModelGeometry(const Entity& entity, std::vector<Triangle>* const out_triangles);
};

}// end namespace ph