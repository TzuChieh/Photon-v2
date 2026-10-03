#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Utility/TSpan.h"

#include <Common/Utility/TFunction.h>

namespace ph
{

class Ray;
class HitProbe;
class Intersectable;

class Intersector : public Intersectable
{
public:
	using HitVisitor = TFunction<void(const Ray& ray, const HitProbe& probe)>;

	virtual void update(TSpanView<const Intersectable*> intersectables) = 0;
	
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override = 0;

	math::AABB3D calcAABB() const override = 0;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	/*! @brief Visit all hits without shortening the query segment.
	The visitor receives the ray and probe for each hit, and visitation order is unspecified.
	Ray and probe references are valid only during the call.
	@exception IllegalOperationException If `supportsForEachIntersection()` is false.
	*/
	virtual void forEachIntersection(const Ray& ray, const HitVisitor& visitor) const;

	/*! @brief Whether `forEachIntersection()` is supported. Defaults to false.
	*/
	virtual bool supportsForEachIntersection() const;

protected:
	static void forEachIntersectionInTarget(
		const Intersectable& target,
		const Ray& ray,
		const HitVisitor& visitor);
};

}// end namespace ph
