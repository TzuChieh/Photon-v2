#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Math/Geometry/TAABB3D.h"

#include <Common/assertion.h>

namespace ph
{

class PEmpty : public Primitive
{
public:
	PEmpty();

	bool isIntersecting(HitEvent& event) const override;

	bool reintersect(HitEvent& event, HitEvent& srcEvent) const override;

	void calcHitDetail(HitEvent& event, HitDetail* out_detail) const override;

	bool isOccluding(const Ray& ray) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
};

// In-header Implementation:

inline PEmpty::PEmpty() : 
	Primitive()
{}

inline bool PEmpty::isIntersecting(HitEvent& event) const
{
	return false;
}

inline bool PEmpty::isOccluding(const Ray& ray) const
{
	return false;
}

inline bool PEmpty::reintersect(HitEvent& /* event */, HitEvent& /* srcEvent */) const
{
	// Neither the destination ray nor the source ray can hit an empty primitive.
	return false;
}

inline void PEmpty::calcHitDetail(HitEvent& event, HitDetail* const out_detail) const
{
	// An empty always returns false for intersection probing, it is therefore
	// impossible to have this method called with a valid `HitEvent` instance.
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
