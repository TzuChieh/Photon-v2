#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class NormalBufferIntegrator final : public Integrator, public TCommandInterface<NormalBufferIntegrator>
{
public:
	virtual ~NormalBufferIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;

// command interface
public:
	NormalBufferIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<NormalBufferIntegrator> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<NormalBufferIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph