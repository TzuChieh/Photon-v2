#pragma once

#include "Engine/Core/Intersection/PBasicSphere.h"
#include "Engine/Math/math_fwd.h"

namespace ph
{

class PLatLong01Sphere : public PBasicSphere
{
public:
	explicit PLatLong01Sphere(real radius);

	void calcHitDetail(HitEvent& event, HitDetail* out_detail) const override;

	void genPosSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override;

	void calcPosPdf(PrimitivePosPdfQuery& query) const override;

	/*! @brief Maps a position on the sphere surface to UV coordinates (lat-long-01 format).
	*/
	math::Vector2R posToUV(const math::Vector3R& position) const;
};

}// end namespace ph
