#pragma once

#include "Entity/Primitive/Primitive.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Entity;

class Geometry
{
public:
	virtual ~Geometry() = 0;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Entity* const parentEntity) const = 0;
};

}// end namespace ph