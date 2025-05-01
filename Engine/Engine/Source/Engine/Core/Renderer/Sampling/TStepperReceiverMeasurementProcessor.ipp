#pragma once

#include "Engine/Core/Renderer/Sampling/TStepperReceiverMeasurementProcessor.h"
#include "Engine/Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

template<typename Estimation>
inline TStepperReceiverMeasurementProcessor<Estimation>::
TStepperReceiverMeasurementProcessor(
	const std::size_t numEstimations,
	Integrand integrand,
	std::vector<std::shared_ptr<typename Parent::FilmType>> films)

	: TReceiverMeasurementProcessor<Estimation>(
		numEstimations,
		std::move(integrand),
		std::move(films)),

	, m_filmStepSizes(films.size(), 1)
	, m_currentBatchNumber(0)
{}

template<typename Estimation>
inline void TStepperReceiverMeasurementProcessor<Estimation>::
onBatchStart(const uint64 batchNumber)
{
	m_currentBatchNumber = batchNumber;
}

template<typename Estimation>
inline void TStepperReceiverMeasurementProcessor<Estimation>::
process(
	const math::Vector2D& rasterCoord,
	const Ray&            ray, 
	const math::Spectrum& quantityWeight,
	SampleFlow&           sampleFlow)
{
	for(const auto* estimator : Parent::m_estimators)
	{
		estimator->estimate(ray, Parent::m_integrand, sampleFlow, Parent::m_estimations);
	}

	for(const auto& [estimationIdx, filmIdx] : Parent::m_estimationToFilm)
	{
		if(m_currentBatchNumber % m_filmStepSizes[filmIdx] == 0)
		{
			Parent::m_films[filmIdx].addSample(rasterCoord.x(), rasterCoord.y(), Parent::m_estimations[estimationIdx] * quantityWeight);
		}
	}
}

template<typename Estimation>
inline void TStepperReceiverMeasurementProcessor<Estimation>::
setFilmStepSize(
	const std::size_t filmIndex,
	const std::size_t stepSize)
{
	PH_ASSERT_LT(filmIndex, Parent::m_films.size());
	PH_ASSERT_GE(stepSize, 1);

	m_filmStepSizes[filmIndex] = stepSize;
}

}// end namespace ph
