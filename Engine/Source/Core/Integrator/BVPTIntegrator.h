#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

/*
	BVPT: Backward Vanilla Path Tracing

	This integrator is the basic version of path tracing. Rays shot from
	the camera, bouncing around the scene until an emitter is hit, gather
	its emitted power and keep bouncing... Most online tutorials 
	implemented this GI algorithm. Since this integrator is relatively
	simple but still unbiased, it is good for ground truth rendering if 
	the correctness of another integrator is in doubt.
*/
class BVPTIntegrator final : public Integrator, public TCommandInterface<BVPTIntegrator>
{
public:
	virtual ~BVPTIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;

// command interface
public:
	BVPTIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BVPTIntegrator> ciLoad(const InputPacket& packet);
};

}// end namespace ph