#pragma once

#include "Core/Estimator/IRayEnergyEstimator.h"

#include <cstddef>
#include <iostream>

namespace ph
{

class PartialRayEnergyEstimator : public IRayEnergyEstimator
{
public:
	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		EnergyEstimation& out_estimation) const override = 0;

	void mapAttributeToEstimation(
		std::size_t attributeId,
		std::size_t estimationIndex) override;

	void setEstimationIndex(std::size_t index);

	std::string toString() const override;

protected:
	std::size_t m_estimationIndex;
};

// In-header Implementations:

inline void PartialRayEnergyEstimator::mapAttributeToEstimation(
	const std::size_t attributeId,
	const std::size_t estimationIndex)
{
	if(attributeId == 0)
	{
		setEstimationIndex(estimationIndex);
	}
	else
	{
		std::cerr <<
			"warning: at PartialRayEnergyEstimator::mapAttributeToEstimation(), "
			"attempting to set estimation index for non-existent attribute ID " << attributeId << std::endl;
	}
}

inline void PartialRayEnergyEstimator::setEstimationIndex(const std::size_t index)
{
	m_estimationIndex = index;
}

inline std::string PartialRayEnergyEstimator::toString() const
{
	return "Partial Ray Energy Estimator";
}

}// end namespace ph