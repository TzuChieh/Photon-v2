#pragma once

#include "Core/Integrator.h"

namespace ph
{

class BackwardPathIntegrator final : public Integrator
{
public:
	virtual ~BackwardPathIntegrator() override;

	virtual void cook(const World& world) override;
	virtual void radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const override;
};

}// end namespace ph