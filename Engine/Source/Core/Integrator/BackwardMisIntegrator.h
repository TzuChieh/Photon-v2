#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

class BackwardMisIntegrator final : public Integrator, public TCommandInterface<BackwardMisIntegrator>
{
public:
	virtual ~BackwardMisIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(Vector3R& value);
	static real misWeight(real pdf1W, real pdf2W);

// command interface
public:
	BackwardMisIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<BackwardMisIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph