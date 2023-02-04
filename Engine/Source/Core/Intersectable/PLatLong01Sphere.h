#pragma once

#include "Core/Intersectable/PBasicSphere.h"
#include "Math/math_fwd.h"

namespace ph
{

class PLatLong01Sphere : public PBasicSphere
{
public:
	explicit PLatLong01Sphere(real radius);

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	void genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override;

	/*! @brief Maps a position on the sphere surface to UV coordinates (lat-long-01 format).
	*/
	math::Vector2R positionToUV(const math::Vector3R& position) const;
};

}// end namespace ph
