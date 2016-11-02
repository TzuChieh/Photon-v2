#pragma once

#include "Model/Material/SurfaceIntegrand.h"

namespace ph
{

class MatteOpaque;

class LambertianDiffuseSurfaceIntegrand : public SurfaceIntegrand
{
public:
	LambertianDiffuseSurfaceIntegrand(const MatteOpaque* const matteOpaque);
	virtual ~LambertianDiffuseSurfaceIntegrand() override;

	virtual void genUniformRandomVOverRegion(const Vector3f& N, Vector3f* out_V) const override;
	virtual void sampleBRDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_BRDF) const override;

private:
	const MatteOpaque* m_matteOpaque;
};

}// end namespace ph