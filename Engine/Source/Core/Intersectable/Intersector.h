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
	virtual ~Intersector() = 0;

	// FIXME: should update with intersectables only
	virtual void update(const CookedDataStorage& cookedActors) = 0;
	
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const = 0;

	virtual void calcAABB(AABB3D* out_aabb) const = 0;
	
	using Intersectable::isIntersecting;
	virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                                    HitDetail* out_detail) const override;
};

}// end namespace ph