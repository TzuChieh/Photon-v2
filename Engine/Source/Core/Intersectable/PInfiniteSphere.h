#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Math/math_fwd.h"
#include "Common/primitive_type.h"

namespace ph
{

// FIXME: flawed logic

class PInfiniteSphere : public Primitive
{
public:
	PInfiniteSphere(
		real effectivelyInfiniteRadius, 
		const PrimitiveMetadata* metadata);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool isIntersecting(const Ray& ray) const override;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool isIntersectingVolumeConservative(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;

	bool uvwToPosition(
		const math::Vector3R& uvw,
		const math::Vector3R& observationPoint,
		math::Vector3R*       out_position) const override;

private:
	real m_effectivelyInfiniteRadius;
};

}// end namespace ph
