#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Math/Geometry/TAABB3D.h"
#include "Common/assertion.h"

namespace ph
{

class PEmpty : public Primitive
{
public:
	explicit PEmpty(const PrimitiveMetadata* metadata);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool isIntersecting(const Ray& ray) const override;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool isIntersectingVolumeConservative(const math::AABB3D& volume) const override;
	void calcAABB(math::AABB3D* out_aabb) const override;
};

// In-header Implementation:

inline PEmpty::PEmpty(const PrimitiveMetadata* const metadata) : 
	Primitive(metadata)
{}

inline bool PEmpty::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	return false;
}

inline bool PEmpty::isIntersecting(const Ray& ray) const
{
	return false;
}

inline void PEmpty::calcIntersectionDetail(
	const Ray& ray,
	HitProbe&  probe,
	HitDetail* const out_detail) const
{
	// An empty always returns false for intersection probing, it is therefore
	// impossible to have this method called with a valid HitProbe instance.
	PH_ASSERT_UNREACHABLE_SECTION();
}

inline bool PEmpty::isIntersectingVolumeConservative(const math::AABB3D& volume) const
{
	return false;
}

inline void PEmpty::calcAABB(math::AABB3D* const out_aabb) const
{
	// TODO: return a point or an invalid AABB?

	PH_ASSERT(out_aabb);

	*out_aabb = math::AABB3D({0, 0, 0});
}

}// end namespace ph
