#pragma once

#include "Engine/Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

class PathEnergyEstimator : public IRayEnergyEstimator
{
public:
	PathEnergyEstimator();

	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		SampleFlow&       sampleFlow,
		EnergyEstimation& out_estimation) const override = 0;

	void setEstimationIndex(
		EEstimatorAttribute attribute,
		int estimationIdx) override;

	std::unique_ptr<TIRayEstimator<math::Spectrum>> makeCopy() const override = 0;

	std::string toString() const override;

protected:
	int m_estimationIdx;
};

}// end namespace ph
