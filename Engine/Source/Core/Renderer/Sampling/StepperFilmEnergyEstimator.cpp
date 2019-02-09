#include "Core/Renderer/Sampling/StepperFilmEnergyEstimator.h"
#include "Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

StepperFilmEnergyEstimator::StepperFilmEnergyEstimator(
	const std::size_t  numEstimations,
	const std::size_t  numFilms,
	Integrand          integrand,
	SampleFilter       filter) : 

	FilmEnergyEstimator(
		numEstimations,
		numFilms,
		std::move(integrand),
		std::move(filter)),

	m_filmStepSizes     (numFilms, 1),
	m_currentBatchNumber(0)
{}

void StepperFilmEnergyEstimator::onBatchStart(const uint64 batchNumber)
{
	m_currentBatchNumber = batchNumber;
}

void StepperFilmEnergyEstimator::process(const Vector2D& filmNdc, const Ray& ray)
{
	PH_ASSERT_EQ(m_estimation.numEstimations(), m_films.size());

	for(const IRayEnergyEstimator* estimator : m_estimators)
	{
		estimator->estimate(ray, m_integrand, m_estimation);
	}

	const Vector2D rasterPos = filmNdc * m_filmActualResFPx;
	for(const auto& estimationToFilm : m_estimationToFilm)
	{
		const std::size_t filmIndex       = estimationToFilm.second;
		const std::size_t estimationIndex = estimationToFilm.first;

		if(m_currentBatchNumber % m_filmStepSizes[filmIndex] == 0)
		{
			m_films[filmIndex].addSample(rasterPos.x, rasterPos.y, m_estimation[estimationIndex]);
		}
	}
}

}// end namespace ph