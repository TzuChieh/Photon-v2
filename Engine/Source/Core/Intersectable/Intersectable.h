#pragma once

namespace ph
{

class Ray;
class Intersection;
class AABB;

class Intersectable
{
public:
	virtual ~Intersectable() = 0;

	virtual bool isIntersecting(const Ray& ray,
	                            Intersection* out_intersection) const = 0;
	virtual bool isIntersecting(const Ray& ray) const = 0;
	virtual bool isIntersectingVolumeConservative(const AABB& aabb) const = 0;
	virtual void calcAABB(AABB* out_aabb) const = 0;
};

}// end namespace ph