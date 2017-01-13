#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardPathIntegrator final : public Integrator
{
public:
	virtual ~BackwardPathIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;
};

}// end namespace ph