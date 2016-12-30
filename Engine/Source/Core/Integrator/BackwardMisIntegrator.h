#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardMisIntegrator final : public Integrator
{
public:
	virtual ~BackwardMisIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const override;
};

}// end namespace ph