#pragma once

#include "Engine/Core/Estimator/IRayEnergyEstimator.h"
#include "Engine/Core/Estimator/PTEstimatorParams.h"

namespace ph
{

class PathEnergyEstimator : public IRayEnergyEstimator
{
public:
	PathEnergyEstimator();

	virtual void setPTParams(PTEstimatorParams params);

	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		SampleFlow&       sampleFlow,
		EnergyEstimation& out_estimation) override = 0;

	void setEstimationIndex(
		EEstimatorAttribute attribute,
		int estimationIdx) override;

	std::unique_ptr<TIRayEstimator<math::Spectrum>> makeCopy() const override = 0;

	std::string toString() const override;

	const PTEstimatorParams& getPTParams() const;
	int getPathEnergyIndex() const;

private:
	PTEstimatorParams m_params;
	int m_estimationIdx;
};

inline const PTEstimatorParams& PathEnergyEstimator::getPTParams() const
{
	return m_params;
}

inline int PathEnergyEstimator::getPathEnergyIndex() const
{
	return m_estimationIdx;
}

}// end namespace ph
