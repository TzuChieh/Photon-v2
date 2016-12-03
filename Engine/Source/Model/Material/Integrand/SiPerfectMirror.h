#pragma once

#include "Model/Material/Integrand/SurfaceIntegrand.h"

namespace ph
{

class SiPerfectMirror : public SurfaceIntegrand
{
public:
	virtual ~SiPerfectMirror() override;

	virtual void genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_BRDF) const override;

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;
};

}// end namespace ph