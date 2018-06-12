#pragma once

#include "Core/Integrator/AbstractPathIntegrator.h"

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
class BVPTIntegrator final : public AbstractPathIntegrator, public TCommandInterface<BVPTIntegrator>
{
public:
	BVPTIntegrator();
	BVPTIntegrator(const BVPTIntegrator& other);

	std::unique_ptr<Integrator> makeReproduction() const override;

	BVPTIntegrator& operator = (BVPTIntegrator rhs);

	friend void swap(BVPTIntegrator& first, BVPTIntegrator& second);

private:
	void tracePath(
		const Ray&        ray,
		SpectralStrength* out_lightEnergy,
		SurfaceHit*       out_firstHit) const override;

// command interface
public:
	BVPTIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BVPTIntegrator> ciLoad(const InputPacket& packet);
};

}// end namespace ph