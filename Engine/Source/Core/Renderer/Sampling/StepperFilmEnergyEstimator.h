#pragma once

#include "Core/Renderer/Sampling/FilmEnergyEstimator.h"
#include "Core/Filmic/HdrRgbFilm.h"

#include <cstddef>
#include <vector>

namespace ph
{

class StepperFilmEnergyEstimator : public FilmEnergyEstimator
{
public:
	StepperFilmEnergyEstimator() = default;
	StepperFilmEnergyEstimator(
		std::size_t  numEstimations,
		std::size_t  numFilms,
		Integrand    integrand,
		SampleFilter filter);
	StepperFilmEnergyEstimator(StepperFilmEnergyEstimator&& other) = default;

	void onBatchStart(uint64 batchNumber) override;
	void process(const Vector2D& filmNdc, const Ray& ray) override;

	void setFilmStepSize(std::size_t filmIndex, std::size_t stepSize);

	StepperFilmEnergyEstimator& operator = (StepperFilmEnergyEstimator&& other) = default;

private:
	std::vector<std::size_t> m_filmStepSizes;
	uint64                   m_currentBatchNumber;
};

// In-header Implementations:

inline void StepperFilmEnergyEstimator::setFilmStepSize(
	const std::size_t filmIndex, 
	const std::size_t stepSize)
{
	PH_ASSERT_LT(filmIndex, m_films.size());
	PH_ASSERT_GE(stepSize,  1);

	m_filmStepSizes[filmIndex] = stepSize;
}

}// end namespace ph