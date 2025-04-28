#pragma once

#include "Engine/Core/Estimator/PathEnergyEstimator.h"

namespace ph
{

/*! @brief A direct lighting only estimator using path tracing.

BVPTDL: Backward Vanilla Path Tracing Direct Lighting.

This estimator has properties similar to BVPT, excpet that it computes only
direct lighting effects.
*/
class BVPTDLEstimator : public PathEnergyEstimator
{
public:
	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		SampleFlow&       sampleFlow,
		EnergyEstimation& out_estimation) const override;

	std::unique_ptr<TIRayEstimator<math::Spectrum>> makeCopy() const override;

	std::string toString() const override;
};

}// end namespace ph
