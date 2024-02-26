#pragma once

#include "Core/Intersection/PBasicSphere.h"
#include "Math/math_fwd.h"

namespace ph
{

class PLatLong01Sphere : public PBasicSphere
{
public:
	explicit PLatLong01Sphere(real radius);

	void calcHitDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	void genPosSample(
		PrimitivePosSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcPosSamplePdfA(
		PrimitivePosSamplePdfQuery& query,
		HitProbe& probe) const override;

	/*! @brief Maps a position on the sphere surface to UV coordinates (lat-long-01 format).
	*/
	math::Vector2R positionToUV(const math::Vector3R& position) const;
};

}// end namespace ph
