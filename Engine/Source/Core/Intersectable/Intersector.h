#pragma once

#include "Core/Intersectable/Intersectable.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;
class CookedActorStorage;

class Intersector : public Intersectable
{
public:
	virtual ~Intersector() = 0;

	virtual void update(const CookedActorStorage& cookedActors) = 0;

	virtual bool isIntersecting(const Ray& ray, 
	                            Intersection* out_intersection) const = 0;

	virtual void calcAABB(AABB3D* out_aabb) const = 0;
};

}// end namespace ph