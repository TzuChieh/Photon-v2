#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;
class CookedActorStorage;

class Intersector
{
public:
	virtual ~Intersector() = 0;

	virtual void update(const CookedActorStorage& cookedActors) = 0;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const = 0;
	virtual bool isIntersecting(const Ray& ray) const = 0;
};

}// end namespace ph