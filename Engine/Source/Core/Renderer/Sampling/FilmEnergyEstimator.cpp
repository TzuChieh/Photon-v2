#include "Core/Renderer/Sampling/FilmEnergyEstimator.h"
#include "Math/TVector2.h"
#include "Core/Ray.h"
#include "Core/Filmic/SampleFilters.h"
#include "Core/Estimator/IRayEnergyEstimator.h"

#include <iostream>

namespace ph
{

FilmEnergyEstimator::FilmEnergyEstimator(
	const std::size_t numEstimations,
	Integrand integrand,
	const Vector2S& filmActualResPx,
	const TAABB2D<int64>& filmEffectiveWindowPx,
	const SampleFilter& filter) :

	m_films(numEstimations),
	m_estimation(numEstimations),
	m_estimators(),
	m_integrand(std::move(integrand))
{
	for(HdrRgbFilm& film : m_films)
	{
		film = HdrRgbFilm(
			static_cast<int64>(filmActualResPx.x),
			static_cast<int64>(filmActualResPx.y),
			filmEffectiveWindowPx,
			filter);
	}

	std::cerr << "fee " << m_films[0].getEffectiveResPx().toString() << std::endl;
}

FilmEnergyEstimator::FilmEnergyEstimator(FilmEnergyEstimator&& other) : 
	m_films(std::move(other.m_films)),
	m_estimation(std::move(other.m_estimation)),
	m_estimators(std::move(other.m_estimators)),
	m_integrand(std::move(other.m_integrand))
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
		const Vector2D rasterPos = sensorNdc * Vector2D(m_films[i].getActualResPx());

		m_films[i].addSample(rasterPos.x, rasterPos.y, m_estimation[i]);
	}

	//std::cerr << "fee " << m_films[0].getEffectiveResPx().toString() << std::endl;
}

void FilmEnergyEstimator::addEstimator(const IRayEnergyEstimator* const estimator)
{
	PH_ASSERT(estimator);

	m_estimators.push_back(estimator);
}

FilmEnergyEstimator& FilmEnergyEstimator::operator = (FilmEnergyEstimator&& other)
{
	TSensedRayProcessor<FilmEnergyEstimator>::operator = (std::move(other));

	m_films = std::move(other.m_films);
	m_estimation = std::move(other.m_estimation);
	m_estimators = std::move(other.m_estimators);
	m_integrand = std::move(other.m_integrand);

	return *this;
}

}// end namespace ph