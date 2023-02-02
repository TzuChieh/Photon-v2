#pragma once

#include "Core/Intersectable/PBasicSphere.h"

namespace ph
{

class PLatLong01Sphere : public PBasicSphere
{
public:
	explicit PLatLong01Sphere(real radius);

	math::Vector2R positionToUV(const math::Vector3R& position) const override;

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;
};

}// end namespace ph
