#pragma once

#include "Core/Intersection/Primitive.h"
#include "Math/TVector3.h"

#include <Common/primitive_type.h>

#include <utility>

namespace ph
{

class PBasicSphere : public Primitive
{
public:
	explicit PBasicSphere(real radius);

	void calcHitDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override = 0;

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcExtendedArea() const override;

	real getRadius() const;
	real getRcpRadius() const;

protected:
	/*!
	Get refined surface position since the original coordinates may contain large numerical error
	due to far ray, extreme-sized sphere, etc.
	@return Refined surface position and the normal of that position. Normal is pointing away from
	the sphere.
	*/
	auto getRefinedSurfaceAndNormal(const math::Vector3R& srcSurface) const
	-> std::pair<math::Vector3R, math::Vector3R>;

private:
	real m_radius;
	real m_rcpRadius;
};

// In-header Implementations:

inline real PBasicSphere::getRadius() const
{
	return m_radius;
}

inline real PBasicSphere::getRcpRadius() const
{
	return m_rcpRadius;
}

inline auto PBasicSphere::getRefinedSurfaceAndNormal(const math::Vector3R& srcSurface) const
-> std::pair<math::Vector3R, math::Vector3R>
{
	const auto refinedNormal = srcSurface.safeNormalize({0, 1, 0});
	const auto refinedSurface = refinedNormal * getRadius();
	return {refinedSurface, refinedNormal};
}

}// end namespace ph
