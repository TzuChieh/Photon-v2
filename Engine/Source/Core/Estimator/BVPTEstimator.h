#pragma once

#include "Core/Estimator/FullEnergyEstimator.h"

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
class BVPTEstimator : public FullEnergyEstimator
{
public:
	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		EnergyEstimation& out_estimation) const override;
};

// In-header Implementations:

inline void BVPTEstimator::update(const Integrand& integrand)
{}

}// end namespace ph