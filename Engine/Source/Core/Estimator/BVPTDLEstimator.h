#pragma once

#include "Core/Estimator/PartialRayEnergyEstimator.h"

namespace ph
{

/*
	BVPTDL: Backward Vanilla Path Tracing Direct Lighting

	This estimator has properties similar to BVPT, excpet that it computes only
	direct lighting effects.
*/
class BVPTDLEstimator : public PartialRayEnergyEstimator
{
public:
	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		EnergyEstimation& out_estimation) const override;

	std::string toString() const override;
};

// In-header Implementations:

inline void BVPTDLEstimator::update(const Integrand& integrand)
{}

inline std::string BVPTDLEstimator::toString() const
{
	return "Backward Vanilla Path Tracing Direct Lighting Estimator";
}

}// end namespace ph
