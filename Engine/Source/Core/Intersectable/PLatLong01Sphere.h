#pragma once

#include "Core/Intersectable/PBasicSphere.h"

namespace ph
{

class PLatLong01Sphere : public PBasicSphere
{
public:
	PLatLong01Sphere(const PrimitiveMetadata* metadata, real radius);

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;
};

}// end namespace ph
