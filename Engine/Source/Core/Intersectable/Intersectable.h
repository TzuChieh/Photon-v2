#pragma once

namespace ph
{

class Ray;
class Intersection;
class AABB3D;

class Intersectable
{
public:
	virtual ~Intersectable() = 0;

	virtual bool isIntersecting(const Ray& ray,
	                            Intersection* out_intersection) const = 0;
	virtual bool isIntersecting(const Ray& ray) const = 0;
	virtual bool isIntersectingVolumeConservative(const AABB3D& aabb) const = 0;
	virtual void calcAABB(AABB3D* out_aabb) const = 0;
};

}// end namespace ph