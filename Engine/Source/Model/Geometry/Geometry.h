#pragma once

namespace ph
{

class Ray;
class Intersection;

class Geometry
{
public:
	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const = 0;
};

}// end namespace ph