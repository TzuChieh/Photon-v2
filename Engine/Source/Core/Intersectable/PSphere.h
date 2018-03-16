#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/primitive_type.h"

namespace ph
{

class PSphere final : public Primitive
{
public:
	PSphere(const PrimitiveMetadata* metadata, real radius);
	virtual ~PSphere() override;

	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                                    HitDetail* out_detail) const override;
	virtual bool isIntersectingVolumeConservative(const AABB3D& volume) const override;
	virtual void calcAABB(AABB3D* out_aabb) const override;
	virtual real calcPositionSamplePdfA(const Vector3R& position) const override;
	virtual void genPositionSample(PositionSample* out_sample) const override;

	virtual real calcExtendedArea() const override;

private:
	real m_radius;
};

}// end namespace ph