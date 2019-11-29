#pragma once

#include "Core/Renderer/Sampling/TCameraMeasurementEstimator.h"
#include "Math/TVector2.h"
#include "Core/Ray.h"
#include "Core/Filmic/SampleFilters.h"
#include "Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

template<typename SamplingFilmType, typename EstimationType>
inline TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
TCameraMeasurementEstimator(
	const std::size_t  numFilms,
	const std::size_t  numEstimations,
	Integrand          integrand,
	SampleFilter       filter) :

	m_filter          (std::move(filter)),
	m_estimations     (numEstimations),
	m_filmActualResFPx(0),
	m_films           (numFilms),
	m_estimators      (),
	m_integrand       (std::move(integrand)),
	m_estimationToFilm()
{
	for(SamplingFilmType& film : m_films)
	{
		film = SamplingFilmType(
			0,
			0,
			{{0, 0}, {0, 0}},
			m_filter);
	}
}

template<typename SamplingFilmType, typename EstimationType>
inline TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
TCameraMeasurementEstimator(TCameraMeasurementEstimator&& other) :
	m_filter          (std::move(other.m_filter)),
	m_estimations     (std::move(other.m_estimations)),
	m_filmActualResFPx(std::move(other.m_filmActualResFPx)),
	m_films           (std::move(other.m_films)),
	m_estimators      (std::move(other.m_estimators)),
	m_integrand       (std::move(other.m_integrand)),
	m_estimationToFilm(std::move(other.m_estimationToFilm))
{}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
process(
	const math::Vector2D& filmNdc, 
	const Ray&            sensedRay, 
	SampleFlow&           sampleFlow)
	-> void
{
	for(const auto* estimator : m_estimators)
	{
		estimator->estimate(sensedRay, m_integrand, sampleFlow, m_estimations);
	}

	const math::Vector2D rasterPos = filmNdc * m_filmActualResFPx;
	for(const auto& estimationToFilm : m_estimationToFilm)
	{
		const std::size_t estimationIndex = estimationToFilm.first;
		const std::size_t filmIndex       = estimationToFilm.second;

		m_films[filmIndex].addSample(rasterPos.x, rasterPos.y, m_estimations[estimationIndex]);
	}
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
addEstimator(const Estimator* const estimator)
	-> void
{
	PH_ASSERT(estimator);

	m_estimators.push_back(estimator);
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
addFilmEstimation(
	const std::size_t filmIndex,
	const std::size_t estimationIndex)
	-> void
{
	PH_ASSERT_LT(filmIndex, m_films.size());
	PH_ASSERT_LT(estimationIndex, m_estimations.numEstimations());

	m_estimationToFilm.push_back({estimationIndex, filmIndex});
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
clearFilms()
	-> void
{
	for(std::size_t i = 0; i < m_films.size(); ++i)
	{
		clearFilm(i);
	}
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
clearFilm(const std::size_t index)
	-> void
{
	PH_ASSERT_LT(index, m_films.size());

	m_films[index].clear();
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
mergeFilmTo(const std::size_t fromIndex, SamplingFilmType& toFilm)
	-> void
{
	PH_ASSERT_LT(fromIndex, m_films.size());

	toFilm.mergeWith(m_films[fromIndex]);
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
setFilmDimensions(
	const math::TVector2<int64>& actualResPx,
	const math::TAABB2D<int64>&  effectiveWindowPx,
	const bool                   useSoftEdge)
	-> void
{
	m_filmActualResFPx = math::Vector2D(actualResPx);

	for(SamplingFilmType& film : m_films)
	{
		film.setActualResPx(actualResPx);
		film.setEffectiveWindowPx(effectiveWindowPx);
		film.setSoftEdge(useSoftEdge);
	}
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
numEstimations() const
	-> std::size_t
{
	return m_estimations.numEstimations();
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
getFilmEffectiveWindowPx() const
	-> math::TAABB2D<int64>
{
	PH_ASSERT(!m_films.empty());

	return m_films.front().getEffectiveWindowPx();
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
getFilmDimensions() const
	-> SamplingFilmDimensions
{
	PH_ASSERT(!m_films.empty());

	return m_films.front().getDimensions();
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
isSoftEdgedFilm() const
	-> bool
{
	PH_ASSERT(!m_films.empty());

	return m_films.front().isSoftEdge();
}

template<typename SamplingFilmType, typename EstimationType>
inline auto TCameraMeasurementEstimator<SamplingFilmType, EstimationType>::
operator = (TCameraMeasurementEstimator&& other)
	-> TCameraMeasurementEstimator&
{
	ISensedRayProcessor::operator = (std::move(other));

	m_filter           = std::move(other.m_filter);
	m_estimations      = std::move(other.m_estimations);
	m_filmActualResFPx = std::move(other.m_filmActualResFPx);
	m_films            = std::move(other.m_films);
	m_estimators       = std::move(other.m_estimators);
	m_integrand        = std::move(other.m_integrand);
	m_estimationToFilm = std::move(other.m_estimationToFilm);

	return *this;
}

}// end namespace ph
