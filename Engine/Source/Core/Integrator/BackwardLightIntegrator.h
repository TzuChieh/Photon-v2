#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardLightIntegrator final : public Integrator
{
public:
	virtual ~BackwardLightIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(Vector3f& value);
};

}// end namespace ph