#include "Core/Renderer/Sampling/FilmEnergyEstimator.h"
#include "Math/TVector2.h"
#include "Core/Ray.h"
#include "Core/Filmic/SampleFilters.h"
#include "Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

FilmEnergyEstimator::FilmEnergyEstimator(
	const std::size_t numEstimations,
	Integrand         integrand,
	SampleFilter      filter) :

	m_filter          (std::move(filter)),
	m_estimation      (numEstimations),
	m_filmActualResFPx(0),

	m_films           (),
	m_estimators      (),
	m_integrand       (std::move(integrand))
{}

FilmEnergyEstimator::FilmEnergyEstimator(FilmEnergyEstimator&& other) : 
	m_filter          (std::move(other.m_filter)),
	m_estimation      (std::move(other.m_estimation)),
	m_filmActualResFPx(std::move(other.m_filmActualResFPx)),

	m_films           (std::move(other.m_films)),
	m_estimators      (std::move(other.m_estimators)),
	m_integrand       (std::move(other.m_integrand))
{}

void FilmEnergyEstimator::process(const Vector2D& filmNdc, const Ray& ray)
{
	PH_ASSERT_EQ(m_estimation.numEstimations(), m_films.size());

	for(const IRayEnergyEstimator* estimator : m_estimators)
	{
		estimator->estimate(ray, m_integrand, m_estimation);
	}

	for(std::size_t i = 0; i < m_films.size(); ++i)
	{
		const Vector2D rasterPos = filmNdc * m_filmActualResFPx;

		m_films[i].addSample(rasterPos.x, rasterPos.y, m_estimation[i]);
	}
}

void FilmEnergyEstimator::addEstimator(const IRayEnergyEstimator* const estimator)
{
	PH_ASSERT(estimator);

	m_estimators.push_back(estimator);
}

FilmEnergyEstimator& FilmEnergyEstimator::operator = (FilmEnergyEstimator&& other)
{
	ISensedRayProcessor::operator = (std::move(other));

	m_filter           = std::move(other.m_filter);
	m_estimation       = std::move(other.m_estimation);
	m_filmActualResFPx = std::move(other.m_filmActualResFPx);

	m_films            = std::move(other.m_films);
	m_estimators       = std::move(other.m_estimators);
	m_integrand        = std::move(other.m_integrand);

	return *this;
}

void FilmEnergyEstimator::lazilyConstructFilms(
	const TVector2<int64>& actualResPx,
	const TAABB2D<int64>&  effectiveWindowPx,
	const bool             useSoftEdge)
{
	if(m_films.size() != m_estimation.numEstimations())
	{
		m_films.resize(m_estimation.numEstimations());
		for(HdrRgbFilm& film : m_films)
		{
			film = HdrRgbFilm(
				actualResPx.x,
				actualResPx.y,
				effectiveWindowPx,
				m_filter);
		}
	}
	else
	{
		for(HdrRgbFilm& film : m_films)
		{
			film.setActualResPx(actualResPx);
			film.setEffectiveWindowPx(effectiveWindowPx);
		}
	}

	for(HdrRgbFilm& film : m_films)
	{
		film.setSoftEdge(useSoftEdge);
	}
}

}// end namespace ph