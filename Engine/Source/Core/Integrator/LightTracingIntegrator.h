#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class LightTracingIntegrator final : public Integrator
{
public:
	virtual ~LightTracingIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const override;

private:
	static void rationalClamp(Vector3f& value);
};

}// end namespace ph