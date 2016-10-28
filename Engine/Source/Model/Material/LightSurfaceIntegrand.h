#pragma once

#include "Model/Material/SurfaceIntegrand.h"

namespace ph
{

class LightSurfaceIntegrand : public SurfaceIntegrand
{
public:
	virtual ~LightSurfaceIntegrand() override;

	virtual void genUniformRandomLOverRegion(const Vector3f& N, Vector3f* out_L) const override;
	virtual bool sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const override;
};

}// end namespace ph