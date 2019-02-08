#pragma once

#include "Core/Renderer/Sampling/TSensedRayProcessor.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Estimator/estimator_fwd.h"
#include "Common/assertion.h"
#include "Core/Estimator/EnergyEstimation.h"
#include "Core/Estimator/Integrand.h"
#include "Math/math_fwd.h"

#include <vector>
#include <cstddef>

namespace ph
{

class FilmEnergyEstimator : public TSensedRayProcessor<FilmEnergyEstimator>
{
public:
	FilmEnergyEstimator() = default;

	FilmEnergyEstimator(
		std::size_t numEstimations, 
		Integrand integrand,
		const Vector2S& filmActualResPx,
		const TAABB2D<int64>& filmEffectiveWindowPx,
		const SampleFilter& filter);

	FilmEnergyEstimator(FilmEnergyEstimator&& other);

	void impl_process(const Vector2D& sensorNdc, const Ray& ray);

	void addEstimator(const IRayEnergyEstimator* estimator);
	HdrRgbFilm& getFilm(std::size_t index);

	std::size_t numEstimations() const;
	const HdrRgbFilm& getFilm(std::size_t index) const;

	FilmEnergyEstimator& operator = (FilmEnergyEstimator&& other);

private:
	std::vector<HdrRgbFilm>                 m_films;
	EnergyEstimation                        m_estimation;
	std::vector<const IRayEnergyEstimator*> m_estimators;
	Integrand                               m_integrand;
};

// In-header Implementations:

inline HdrRgbFilm& FilmEnergyEstimator::getFilm(const std::size_t index)
{
	PH_ASSERT_LT(index, m_films.size());

	return m_films[index];
}

inline std::size_t FilmEnergyEstimator::numEstimations() const
{
	return m_estimation.numEstimations();
}

inline const HdrRgbFilm& FilmEnergyEstimator::getFilm(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_films.size());

	return m_films[index];
}

}// end namespace ph