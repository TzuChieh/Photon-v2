#pragma once

#include "Engine/Core/Renderer/Sampling/TReceiverMeasurementProcessor.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/Estimator/IRayEnergyEstimator.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

template<typename Estimation>
inline TReceiverMeasurementProcessor<Estimation>::
TReceiverMeasurementProcessor(
	std::size_t numEstimations,
	Integrand integrand,
	std::vector<std::shared_ptr<FilmType>> films)

	: IRasterRayProcessor()
	
	, m_estimations(numEstimations)
	, m_integrand(std::move(integrand))
	, m_films(std::move(films))
	, m_estimators()
	, m_estimationToFilm()
{
	for(const auto& film : m_films)
	{
		PH_ASSERT(film);
	}
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
process(
	const math::Vector2D& rasterCoord,
	const Ray&            sensedRay, 
	const math::Spectrum& quantityWeight,
	SampleFlow&           sampleFlow)
-> void
{
	for(const auto& estimator : m_estimators)
	{
		estimator->estimate(sensedRay, m_integrand, sampleFlow, m_estimations);
	}

	for(const auto& [estimationIdx, filmIdx] : m_estimationToFilm)
	{
		m_films[filmIdx]->addSample(
			rasterCoord.x(),
			rasterCoord.y(),
			m_estimations[estimationIdx] * quantityWeight);
	}
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
addEstimator(std::shared_ptr<EstimatorType> estimator)
-> void
{
	PH_ASSERT(estimator);

	m_estimators.push_back(std::move(estimator));
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
addFilmEstimation(
	const std::size_t filmIndex,
	const std::size_t estimationIndex)
-> void
{
	PH_ASSERT_LT(filmIndex, m_films.size());
	PH_ASSERT_LT(estimationIndex, m_estimations.numEstimations());

	m_estimationToFilm.push_back({estimationIndex, filmIndex});
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
clearFilms()
-> void
{
	for(std::size_t i = 0; i < m_films.size(); ++i)
	{
		clearFilm(i);
	}
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
clearFilm(const std::size_t index)
-> void
{
	PH_ASSERT_LT(index, m_films.size());

	m_films[index]->clear();
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
mergeFilmTo(const std::size_t fromIndex, FilmType& toFilm)
-> void
{
	PH_ASSERT_LT(fromIndex, m_films.size());

	toFilm.mergeWith(*(m_films[fromIndex]));
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
setFilmDimensions(
	const math::TVector2<int64>& actualResPx,
	const math::TAABB2D<int64>& effectiveWindowPx,
	const bool useSoftEdge)
-> void
{
	for(auto& film : m_films)
	{
		film->setActualResPx(actualResPx);
		film->setEffectiveWindowPx(effectiveWindowPx);
		film->setSoftEdge(useSoftEdge);
	}
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
numEstimations() const
-> std::size_t
{
	return m_estimations.numEstimations();
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
getFilmEffectiveWindowPx() const
-> math::TAABB2D<int64>
{
	PH_ASSERT(!m_films.empty());

	return m_films.front()->getEffectiveWindowPx();
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
getFilmDimensions() const
-> SamplingFilmDimensions
{
	PH_ASSERT(!m_films.empty());

	return m_films.front()->getDimensions();
}

template<typename Estimation>
inline auto TReceiverMeasurementProcessor<Estimation>::
isSoftEdgedFilm() const
-> bool
{
	PH_ASSERT(!m_films.empty());

	return m_films.front()->isSoftEdged();
}

}// end namespace ph
