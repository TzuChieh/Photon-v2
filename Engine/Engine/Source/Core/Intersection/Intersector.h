#pragma once

#include "Core/Intersection/Intersectable.h"
#include "Utility/TSpan.h"

namespace ph
{

class Ray;
class HitProbe;
class Intersectable;

class Intersector : public Intersectable
{
public:
	virtual void update(TSpanView<const Intersectable*> intersectables) = 0;
	
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override = 0;
	math::AABB3D calcAABB() const override = 0;

	void calcHitDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;
};

}// end namespace ph
