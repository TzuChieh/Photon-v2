#pragma once

#include "Core/Intersection/Primitive.h"
#include "Math/Geometry/TAABB3D.h"

#include <Common/assertion.h>

namespace ph
{

class PEmpty : public Primitive
{
public:
	PEmpty();

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool isOccluding(const Ray& ray) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
};

// In-header Implementation:

inline PEmpty::PEmpty() : 
	Primitive()
{}

inline bool PEmpty::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	return false;
}

inline bool PEmpty::isOccluding(const Ray& ray) const
{
	return false;
}

inline bool PEmpty::reintersect(
	const Ray& /* ray */,
	HitProbe& /* probe */,
	const Ray& /* srcRay */,
	HitProbe& /* srcProbe */) const
{
	return false;
}

inline void PEmpty::calcHitDetail(
	const Ray& ray,
	HitProbe&  probe,
	HitDetail* const out_detail) const
{
	// An empty always returns false for intersection probing, it is therefore
	// impossible to have this method called with a valid HitProbe instance.
	PH_ASSERT_UNREACHABLE_SECTION();
}

inline bool PEmpty::mayOverlapVolume(const math::AABB3D& volume) const
{
	return false;
}

inline math::AABB3D PEmpty::calcAABB() const
{
	return math::AABB3D::makeEmpty();
}

}// end namespace ph
