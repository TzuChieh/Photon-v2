#pragma once

#include "Core/Estimator/PathEstimator.h"

namespace ph
{

/*
	BVPT: Backward Vanilla Path Tracing

	This estimator is the basic version of path tracing. Rays shot from
	the camera, bouncing around the scene until an emitter is hit, gather
	its emitted power and keep bouncing... Most online tutorials 
	implemented this GI algorithm. Since this estimator is relatively
	simple but still unbiased, it is good for ground truth rendering if 
	the correctness of another estimator is in doubt.
*/
class BVPTEstimator : public PathEstimator, public TCommandInterface<BVPTEstimator>
{
public:
	BVPTEstimator();

	void radianceAlongRay(
		const Ray&        ray,
		const Integrand&  integrand,
		SpectralStrength& out_radiance,
		SurfaceHit&       out_firstHit) const override;

// command interface
public:
	explicit BVPTEstimator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BVPTEstimator> ciLoad(const InputPacket& packet);
};

}// end namespace ph