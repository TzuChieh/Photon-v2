#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/primitive_type.h"

namespace ph
{

class PSphere final : public Primitive
{
public:
	explicit PSphere(const PrimitiveMetadata* metadata, real radius);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,           
		HitDetail* out_detail) const override;

	bool isIntersectingVolumeConservative(const AABB3D& volume) const override;
	void calcAABB(AABB3D* out_aabb) const override;
	real calcPositionSamplePdfA(const Vector3R& position) const override;
	void genPositionSample(PositionSample* out_sample) const override;
	real calcExtendedArea() const override;

private:
	real m_radius;
	real m_reciRadius;
};

}// end namespace ph