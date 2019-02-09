#pragma once

#include "Core/Renderer/Sampling/ISensedRayProcessor.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Estimator/estimator_fwd.h"
#include "Common/assertion.h"
#include "Core/Estimator/EnergyEstimation.h"
#include "Core/Estimator/Integrand.h"
#include "Math/math_fwd.h"
#include "Core/Bound/TAABB2D.h"

#include <vector>
#include <cstddef>

namespace ph
{

class FilmEnergyEstimator : public ISensedRayProcessor
{
public:
	FilmEnergyEstimator() = default;

	FilmEnergyEstimator(
		std::size_t  numEstimations, 
		Integrand    integrand,
		SampleFilter filter);

	FilmEnergyEstimator(FilmEnergyEstimator&& other);

	void process(const Vector2D& filmNdc, const Ray& ray) override;

	void addEstimator(const IRayEnergyEstimator* estimator);
	void setFilmDimensions(
		const TVector2<int64>& actualResPx, 
		const TAABB2D<int64>&  effectiveWindowPx,
		bool                   useSoftEdge = true);
	void clearFilms();
	void clearFilm(std::size_t index);
	void mergeFilmTo(std::size_t fromIndex, HdrRgbFilm& toFilm);

	std::size_t            numEstimations() const;
	TAABB2D<int64>         getFilmEffectiveWindowPx() const;
	SamplingFilmDimensions getFilmDimensions() const;
	bool                   isSoftEdgedFilm() const;

	FilmEnergyEstimator& operator = (FilmEnergyEstimator&& other);

protected:
	SampleFilter                            m_filter;
	EnergyEstimation                        m_estimation;
	Vector2D                                m_filmActualResFPx;

private:
	std::vector<HdrRgbFilm>                 m_films;
	std::vector<const IRayEnergyEstimator*> m_estimators;
	Integrand                               m_integrand;

	void lazilyConstructFilms(
		const TVector2<int64>& actualResPx,
		const TAABB2D<int64>&  effectiveWindowPx,
		bool                   useSoftEdge);
};

// In-header Implementations:

inline void FilmEnergyEstimator::clearFilms()
{
	for(std::size_t i = 0; i < m_films.size(); ++i)
	{
		clearFilm(i);
	}
}

inline void FilmEnergyEstimator::clearFilm(const std::size_t index)
{
	PH_ASSERT_LT(index, m_films.size());

	m_films[index].clear();
}

inline void FilmEnergyEstimator::mergeFilmTo(const std::size_t fromIndex, HdrRgbFilm& toFilm)
{
	PH_ASSERT_LT(fromIndex, m_films.size());

	toFilm.mergeWith(m_films[fromIndex]);
}

inline void FilmEnergyEstimator::setFilmDimensions(
	const TVector2<int64>& actualResPx,
	const TAABB2D<int64>&  effectiveWindowPx,
	const bool             useSoftEdge)
{
	m_filmActualResFPx = Vector2D(actualResPx);

	lazilyConstructFilms(
		actualResPx, 
		effectiveWindowPx, 
		useSoftEdge);
}

inline std::size_t FilmEnergyEstimator::numEstimations() const
{
	return m_estimation.numEstimations();
}

inline TAABB2D<int64> FilmEnergyEstimator::getFilmEffectiveWindowPx() const
{
	PH_ASSERT(!m_films.empty());

	return m_films.front().getEffectiveWindowPx();
}

inline SamplingFilmDimensions FilmEnergyEstimator::getFilmDimensions() const
{
	PH_ASSERT(!m_films.empty());

	return m_films.front().getDimensions();
}

inline bool FilmEnergyEstimator::isSoftEdgedFilm() const
{
	PH_ASSERT(!m_films.empty());

	return m_films.front().isSoftEdge();
}

}// end namespace ph