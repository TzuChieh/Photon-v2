#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardLightIntegrator final : public Integrator
{
public:
	virtual ~BackwardLightIntegrator() override;

	virtual void update(const Intersector& intersector) override;
	virtual void radianceAlongRay(const Ray& ray, const Intersector& intersector, Vector3f* const out_radiance) const override;
};

}// end namespace ph