#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Math/math_fwd.h"

#include <Common/primitive_type.h>

namespace ph
{

class PBasicSphere : public Primitive
{
public:
	explicit PBasicSphere(real radius);

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override = 0;

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcExtendedArea() const override;

	real getRadius() const;
	real getRcpRadius() const;

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

}// end namespace ph
