#pragma once

#include "Core/Primitive/Primitive.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Entity;
class PrimitiveStorage;

class Geometry
{
public:
	virtual ~Geometry() = 0;

	virtual void discretize(PrimitiveStorage* const out_data, const Entity& parentEntity) const = 0;
};

}// end namespace ph