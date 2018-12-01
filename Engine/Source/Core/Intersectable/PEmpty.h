#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"
#include "Core/Bound/TAABB3D.h"

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

	bool isIntersectingVolumeConservative(const AABB3D& volume) const override;
	void calcAABB(AABB3D* out_aabb) const override;
};

// In-header Implementation:

inline PEmpty::PEmpty(const PrimitiveMetadata* const metadata) : 
	Primitive(metadata)
{}

inline bool PEmpty::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	return false;
}

inline void PEmpty::calcAABB(AABB3D* const out_aabb) const
{
	// FIXME: better return an invalid AABB

	PH_ASSERT(out_aabb);

	*out_aabb = AABB3D({0, 0, 0});
}

}// end namespace ph