#pragma once

#include "Model/Material/SurfaceIntegrand.h"

namespace ph
{

class PerfectMirrorSurfaceIntegrand : public SurfaceIntegrand
{
public:
	virtual ~PerfectMirrorSurfaceIntegrand() override;

	virtual void genUniformRandomVOverRegion(const Vector3f& N, Vector3f* out_V) const override;
	virtual void sampleBRDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_BRDF) const override;
};

}// end namespace ph