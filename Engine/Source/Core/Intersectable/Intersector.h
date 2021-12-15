#pragma once

#include "Core/Intersectable/Intersectable.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class HitProbe;
class Primitive;
class CookedDataStorage;

// TODO: rename to ClusteredIntersectable
class Intersector : public Intersectable
{
public:
	// FIXME: should update with intersectables only
	virtual void update(const CookedDataStorage& cookedActors) = 0;
	
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override = 0;
	math::AABB3D calcAABB() const override = 0;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;
};

}// end namespace ph
