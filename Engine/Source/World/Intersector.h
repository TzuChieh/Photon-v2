#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;

class Intersector
{
public:
	virtual ~Intersector() = 0;

	virtual void update(const std::vector<std::unique_ptr<Primitive>>& primitives) = 0;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const = 0;
};

}// end namespace ph