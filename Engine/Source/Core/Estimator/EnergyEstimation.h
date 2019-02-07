#pragma once

#include "Common/assertion.h"
#include "Core/Quantity/SpectralStrength.h"

#include <vector>
#include <cstddef>

namespace ph
{

class EnergyEstimation
{
public:
	EnergyEstimation();
	explicit EnergyEstimation(std::size_t numEstimations);

	std::size_t numEstimations() const;

	SpectralStrength& operator [] (std::size_t index);
	const SpectralStrength& operator [] (std::size_t index) const;

private:
	std::vector<SpectralStrength> m_estimations;
};

// In-header Implementations:

inline EnergyEstimation::EnergyEstimation() : 
	EnergyEstimation(0)
{}

inline EnergyEstimation::EnergyEstimation(const std::size_t numEstimations) :
	m_estimations(numEstimations, SpectralStrength(0))
{}

inline std::size_t EnergyEstimation::numEstimations() const
{
	return m_estimations.size();
}

inline SpectralStrength& EnergyEstimation::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, m_estimations.size());

	return m_estimations[index];
}

inline const SpectralStrength& EnergyEstimation::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_estimations.size());

	return m_estimations[index];
}

}// namespace ph