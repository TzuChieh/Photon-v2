#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Utility/TSpan.h"

#include <Common/Utility/TFunction.h>

namespace ph
{

class Ray;
class HitEvent;
class Intersectable;

class Intersector : public Intersectable
{
public:
	using HitVisitor = TFunction<void(const HitEvent& event)>;

	virtual void update(TSpanView<const Intersectable*> intersectables) = 0;
	
	bool isIntersecting(HitEvent& event) const override = 0;

	math::AABB3D calcAABB() const override = 0;

	bool reintersect(HitEvent& event, HitEvent& srcEvent) const override;

	void calcHitDetail(HitEvent& event, HitDetail* out_detail) const override;

	/*! @brief Visit all hits without shortening the query segment.
	The visitor receives each recorded hit event, and visitation order is unspecified.
	Event references are valid only during the call.
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
