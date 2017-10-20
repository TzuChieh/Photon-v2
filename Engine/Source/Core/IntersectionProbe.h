#pragma once

#include "Common/primitive_type.h"

#include <limits>

namespace ph
{

class Intersectable;
class IntersectionDetail;
class Ray;

class IntersectionProbe final
{
public:
	inline IntersectionProbe() : 
		hitTarget(nullptr), 
		hitRayT(std::numeric_limits<real>::infinity())
	{

	}

	// A convenient method for acquiring details on intersection.
	void calcIntersectionDetail(const Ray& ray, 
	                            IntersectionDetail* out_detail) const;

	inline void set(const Intersectable* hitTarget, 
	                const real hitRayT)
	{
		this->hitTarget = hitTarget;
		this->hitRayT   = hitRayT;
	}

public:
	const Intersectable* hitTarget;
	real                 hitRayT;
};

}// end namespace ph