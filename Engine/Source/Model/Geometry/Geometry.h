#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;
class Model;
class Triangle;

class Geometry
{
public:
	virtual ~Geometry() = 0;

	virtual void discretize(std::vector<Triangle>* const out_triangles, const Model* const parentModel) const = 0;
};

}// end namespace ph