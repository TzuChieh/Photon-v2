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
	virtual void calcAABB(AABB3D* out_aabb) const = 0;

	// If greater performance is desired, you can override the default implementation
	// which simply calls isIntersecting(2) to do the job.
	virtual bool isIntersecting(const Ray& ray) const;

	// The default implementation performs conservative intersecting test using the
	// AABB calculated by calcAABB(). Although false-positives are allowed for this
	// method, providing an implementation with higher accuracy is benefitial for
	// many algorithms depending on this method.
	virtual bool isIntersectingVolumeConservative(const AABB3D& volume) const;
};

}// end namespace ph