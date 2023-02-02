#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class PBasicSphere : public Primitive
{
public:
	explicit PBasicSphere(real radius);

	virtual math::Vector2R positionToUV(const math::Vector3R& position) const = 0;

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override = 0;

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	
	// TODO: rename to mayOverlapVolume
	bool mayIntersectVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	void genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override;
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
