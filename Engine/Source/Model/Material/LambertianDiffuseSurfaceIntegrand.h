#pragma once

#include "Model/Material/SurfaceIntegrand.h"

namespace ph
{

class LambertianDiffuseSurfaceIntegrand : public SurfaceIntegrand
{
public:
	virtual ~LambertianDiffuseSurfaceIntegrand() override;

	virtual void genUniformRandomLOverRegion(const Vector3f& N, Vector3f* out_L) const override;
	virtual bool sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const override;
};

}// end namespace ph