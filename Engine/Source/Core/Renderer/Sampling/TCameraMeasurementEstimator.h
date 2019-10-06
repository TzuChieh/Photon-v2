#pragma once

#include "Core/Renderer/Sampling/ISensedRayProcessor.h"
#include "Core/Estimator/estimator_fwd.h"
#include "Common/assertion.h"
#include "Core/Estimator/TEstimationArray.h"
#include "Core/Estimator/Integrand.h"
#include "Math/math_fwd.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Filmic/SampleFilter.h"

#include <vector>
#include <cstddef>
#include <utility>
#include <type_traits>

namespace ph
{

template<typename SamplingFilmType, typename EstimationType>
class TCameraMeasurementEstimator : public ISensedRayProcessor
{
public:
	using Estimator = TIRayEstimator<EstimationType>;

	template<typename T>
	using TAABB2D = math::TAABB2D<T>;

	TCameraMeasurementEstimator() = default;

	TCameraMeasurementEstimator(
		std::size_t  numFilms,
		std::size_t  numEstimations, 
		Integrand    integrand,
		SampleFilter filter);

	TCameraMeasurementEstimator(TCameraMeasurementEstimator&& other);

	void process(const Vector2D& filmNdc, const Ray& sensedRay) override;

	void addEstimator(const Estimator* estimator);
	void addFilmEstimation(std::size_t filmIndex, std::size_t estimationIndex);
	void setFilmDimensions(
		const TVector2<int64>& actualResPx, 
		const TAABB2D<int64>&  effectiveWindowPx,
		bool                   useSoftEdge = true);
	void clearFilms();
	void clearFilm(std::size_t index);
	void mergeFilmTo(std::size_t fromIndex, SamplingFilmType& toFilm);

	std::size_t            numEstimations() const;
	TAABB2D<int64>         getFilmEffectiveWindowPx() const;
	SamplingFilmDimensions getFilmDimensions() const;
	bool                   isSoftEdgedFilm() const;

	TCameraMeasurementEstimator& operator = (TCameraMeasurementEstimator&& other);

protected:
	using EstimationToFilmMap = std::vector<std::pair<std::size_t, std::size_t>>;

	SampleFilter                     m_filter;
	TEstimationArray<EstimationType> m_estimations;
	Vector2D                         m_filmActualResFPx;
	std::vector<SamplingFilmType>    m_films;
	std::vector<const Estimator*>    m_estimators;
	Integrand                        m_integrand;
	EstimationToFilmMap              m_estimationToFilm;
};

}// end namespace ph

#include "Core/Renderer/Sampling/TCameraMeasurementEstimator.ipp"