#pragma once

#include "Entity/Primitive/Primitive.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class PrimitiveMetadata;

class Geometry
{
public:
	virtual ~Geometry() = 0;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata* const metadata) const = 0;
};

}// end namespace ph