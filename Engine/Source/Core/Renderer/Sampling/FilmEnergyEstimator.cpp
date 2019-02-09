#include "Core/Renderer/Sampling/FilmEnergyEstimator.h"
#include "Math/TVector2.h"
#include "Core/Ray.h"
#include "Core/Filmic/SampleFilters.h"
#include "Core/Estimator/IRayEnergyEstimator.h"

namespace ph
{

FilmEnergyEstimator::FilmEnergyEstimator(
	const std::size_t  numEstimations,
	const std::size_t  numFilms,
	Integrand          integrand,
	SampleFilter       filter) :

	m_filter          (std::move(filter)),
	m_estimation      (numEstimations),
	m_filmActualResFPx(0),
	m_films           (numFilms),
	m_estimators      (),
	m_integrand       (std::move(integrand)),
	m_estimationToFilm()
{
	for(HdrRgbFilm& film : m_films)
	{
		film = HdrRgbFilm(
			0,
			0,
			{{0, 0}, {0, 0}},
			m_filter);
	}
}

FilmEnergyEstimator::FilmEnergyEstimator(FilmEnergyEstimator&& other) : 
	m_filter          (std::move(other.m_filter)),
	m_estimation      (std::move(other.m_estimation)),
	m_filmActualResFPx(std::move(other.m_filmActualResFPx)),
	m_films           (std::move(other.m_films)),
	m_estimators      (std::move(other.m_estimators)),
	m_integrand       (std::move(other.m_integrand)),
	m_estimationToFilm(std::move(other.m_estimationToFilm))
{}

void FilmEnergyEstimator::process(const Vector2D& filmNdc, const Ray& ray)
{
	PH_ASSERT_EQ(m_estimation.numEstimations(), m_films.size());

	for(const IRayEnergyEstimator* estimator : m_estimators)
	{
		estimator->estimate(ray, m_integrand, m_estimation);
	}

	const Vector2D rasterPos = filmNdc * m_filmActualResFPx;
	for(const auto& estimationToFilm : m_estimationToFilm)
	{
		const std::size_t estimationIndex = estimationToFilm.first;
		const std::size_t filmIndex       = estimationToFilm.second;

		m_films[filmIndex].addSample(rasterPos.x, rasterPos.y, m_estimation[estimationIndex]);
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
	m_estimationToFilm = std::move(other.m_estimationToFilm);

	return *this;
}

}// end namespace ph