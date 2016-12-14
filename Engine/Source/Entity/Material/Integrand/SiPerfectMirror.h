#pragma once

#include "Entity/Material/Integrand/SurfaceIntegrand.h"

namespace ph
{

class SiPerfectMirror : public SurfaceIntegrand
{
public:
	virtual ~SiPerfectMirror() override;

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;
};

}// end namespace ph