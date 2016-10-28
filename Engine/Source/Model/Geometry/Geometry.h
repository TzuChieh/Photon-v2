#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;
class Model;

class Geometry
{
public:
	virtual ~Geometry() = 0;

	virtual void genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const = 0;
};

}// end namespace ph