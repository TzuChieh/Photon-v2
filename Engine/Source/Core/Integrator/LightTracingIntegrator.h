#pragma once

#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

// FIXME: this integrator is broken if the scene contains motion

class LightTracingIntegrator final : public Integrator, public TCommandInterface<LightTracingIntegrator>
{
public:
	virtual ~LightTracingIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(Vector3R& value);

// command interface
public:
	LightTracingIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<LightTracingIntegrator> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<LightTracingIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph