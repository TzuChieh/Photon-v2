#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class NormalBufferIntegrator final : Integrator
{
public:
	virtual ~NormalBufferIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const override;
};

}// end namespace ph