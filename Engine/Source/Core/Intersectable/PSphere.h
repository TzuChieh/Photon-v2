#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/primitive_type.h"

namespace ph
{

class PSphere : public Primitive
{
public:
	PSphere(const PrimitiveMetadata* metadata, real radius);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,           
		HitDetail* out_detail) const override;

	bool isIntersectingVolumeConservative(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	void genPositionSample(SampleFlow& sampleFlow, PositionSample* out_sample) const override;
	real calcExtendedArea() const override;

private:
	real m_radius;
	real m_reciRadius;
};

}// end namespace ph
