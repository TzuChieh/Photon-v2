#pragma once

#include "Core/Intersectable/Intersectable.h"

namespace ph
{

class TransformedIntersectable : public Intersectable
{
public:
	virtual bool isIntersecting(const Ray& ray, 
	                            Intersection* const out_intersection) const = 0;
	virtual bool isIntersecting(const Ray& ray) const = 0;
	virtual bool isIntersectingVolumeConservative(const AABB& aabb) const = 0;
	virtual void calcAABB(AABB* out_aabb) const = 0;
};

}// end namespace ph