#pragma once

#include "Core/Renderer/Sampling/TStepperCameraMeasurementEstimator.h"
#include "Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

template<typename SamplingFilmType, typename EstimationType>
inline TStepperCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
TStepperCameraMeasurementEstimator(
	const std::size_t numFilms,
	const std::size_t numEstimations,
	Integrand         integrand,
	SampleFilter      filter) : 

	TCameraMeasurementEstimator<SamplingFilmType, EstimationType>(
		numFilms,
		numEstimations,
		std::move(integrand),
		std::move(filter)),

	m_filmStepSizes     (numFilms, 1),
	m_currentBatchNumber(0)
{}

template<typename SamplingFilmType, typename EstimationType>
inline void TStepperCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
onBatchStart(const uint64 batchNumber)
{
	m_currentBatchNumber = batchNumber;
}

template<typename SamplingFilmType, typename EstimationType>
inline void TStepperCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
process(
	const math::Vector2D& rasterCoord,
	const Ray&            ray, 
	SampleFlow&           sampleFlow)
{
	for(const auto* estimator : Parent::m_estimators)
	{
		estimator->estimate(ray, Parent::m_integrand, sampleFlow, Parent::m_estimations);
	}

	for(const auto& estimationToFilm : Parent::m_estimationToFilm)
	{
		const std::size_t filmIndex       = estimationToFilm.second;
		const std::size_t estimationIndex = estimationToFilm.first;

		if(m_currentBatchNumber % m_filmStepSizes[filmIndex] == 0)
		{
			Parent::m_films[filmIndex].addSample(rasterCoord.x, rasterCoord.y, Parent::m_estimations[estimationIndex]);
		}
	}
}

template<typename SamplingFilmType, typename EstimationType>
inline void TStepperCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
setFilmStepSize(
	const std::size_t filmIndex,
	const std::size_t stepSize)
{
	PH_ASSERT_LT(filmIndex, Parent::m_films.size());
	PH_ASSERT_GE(stepSize, 1);

	m_filmStepSizes[filmIndex] = stepSize;
}

}// end namespace ph
