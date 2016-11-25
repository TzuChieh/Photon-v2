#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Triangle;

class Intersector
{
public:
	virtual ~Intersector() = 0;

	virtual void construct(const std::vector<Triangle>& triangles) = 0;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const = 0;
};

}// end namespace ph