#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class NormalBufferIntegrator final : Integrator
{
public:
	virtual ~NormalBufferIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;
};

}// end namespace ph