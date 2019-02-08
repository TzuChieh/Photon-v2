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

	m_films          (),
	m_estimation     (numEstimations),
	m_estimators     (),
	m_integrand      (std::move(integrand)),
	m_filmActualResPx(),
	m_filter         (std::move(filter))
{}

FilmEnergyEstimator::FilmEnergyEstimator(FilmEnergyEstimator&& other) : 
	m_films          (std::move(other.m_films)),
	m_estimation     (std::move(other.m_estimation)),
	m_estimators     (std::move(other.m_estimators)),
	m_integrand      (std::move(other.m_integrand)),
	m_filmActualResPx(std::move(other.m_filmActualResPx)),
	m_filter         (std::move(other.m_filter))
{}

void FilmEnergyEstimator::impl_process(const Vector2D& sensorNdc, const Ray& ray)
{
	PH_ASSERT_EQ(m_estimation.numEstimations(), m_films.size());

	for(const IRayEnergyEstimator* estimator : m_estimators)
	{
		estimator->estimate(ray, m_integrand, m_estimation);
	}

	for(std::size_t i = 0; i < m_films.size(); ++i)
	{
		const Vector2D rasterPos = sensorNdc * m_filmActualResPx;

		m_films[i].addSample(rasterPos.x, rasterPos.y, m_estimation[i]);
	}
}

void FilmEnergyEstimator::addEstimator(const IRayEnergyEstimator* const estimator)
{
	PH_ASSERT(estimator);

	m_estimators.push_back(estimator);
}

void FilmEnergyEstimator::setFilmDimensions(
	const TVector2<int64>& filmActualResPx,
	const TAABB2D<int64>&  effectiveWindowPx)
{
	if(m_estimation.numEstimations() == 0)
	{
		return;
	}

	// lazily construct films
	if(m_films.size() != m_estimation.numEstimations() || 
	   !m_films.front().getActualResPx().equals(filmActualResPx))
	{
		m_films.resize(m_estimation.numEstimations());
		for(HdrRgbFilm& film : m_films)
		{
			film = HdrRgbFilm(
				filmActualResPx.x,
				filmActualResPx.y,
				effectiveWindowPx,
				m_filter);
		}
	}
	else
	{
		for(HdrRgbFilm& film : m_films)
		{
			film.setEffectiveWindowPx(effectiveWindowPx);
		}
	}

	m_filmActualResPx = Vector2D(filmActualResPx);
}

FilmEnergyEstimator& FilmEnergyEstimator::operator = (FilmEnergyEstimator&& other)
{
	TSensedRayProcessor<FilmEnergyEstimator>::operator = (std::move(other));

	m_films           = std::move(other.m_films);
	m_estimation      = std::move(other.m_estimation);
	m_estimators      = std::move(other.m_estimators);
	m_integrand       = std::move(other.m_integrand);
	m_filmActualResPx = std::move(other.m_filmActualResPx);
	m_filter          = std::move(other.m_filter);

	return *this;
}

}// end namespace ph