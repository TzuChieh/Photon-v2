#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardPathIntegrator final : public Integrator, public TCommandInterface<BackwardPathIntegrator>
{
public:
	virtual ~BackwardPathIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;

// command interface
public:
	BackwardPathIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<BackwardPathIntegrator> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<BackwardPathIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph