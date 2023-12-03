#pragma once

#include "Core/Renderer/Sampling/IReceivedRayProcessor.h"
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
class TReceiverMeasurementEstimator : public IReceivedRayProcessor
{
public:
	using Estimator = TIRayEstimator<EstimationType>;

	TReceiverMeasurementEstimator() = default;

	TReceiverMeasurementEstimator(
		std::size_t  numFilms,
		std::size_t  numEstimations, 
		Integrand    integrand,
		SampleFilter filter);

	TReceiverMeasurementEstimator(TReceiverMeasurementEstimator&& other);

	void process(
		const math::Vector2D& rasterCoord,
		const Ray&            sensedRay, 
		const math::Spectrum& quantityWeight,
		SampleFlow&           sampleFlow) override;

	void addEstimator(const Estimator* estimator);
	void addFilmEstimation(std::size_t filmIndex, std::size_t estimationIndex);
	void setFilmDimensions(
		const math::TVector2<int64>& actualResPx,
		const math::TAABB2D<int64>&  effectiveWindowPx,
		bool                         useSoftEdge = true);
	void clearFilms();
	void clearFilm(std::size_t index);
	void mergeFilmTo(std::size_t fromIndex, SamplingFilmType& toFilm);

	std::size_t numEstimations() const;
	math::TAABB2D<int64> getFilmEffectiveWindowPx() const;
	SamplingFilmDimensions getFilmDimensions() const;
	bool isSoftEdgedFilm() const;

	TReceiverMeasurementEstimator& operator = (TReceiverMeasurementEstimator&& other);

protected:
	using EstimationToFilmMap = std::vector<std::pair<std::size_t, std::size_t>>;

	SampleFilter                     m_filter;
	TEstimationArray<EstimationType> m_estimations;
	math::Vector2D                   m_filmActualResFPx;
	std::vector<SamplingFilmType>    m_films;
	std::vector<const Estimator*>    m_estimators;
	Integrand                        m_integrand;
	EstimationToFilmMap              m_estimationToFilm;
};

}// end namespace ph

#include "Core/Renderer/Sampling/TReceiverMeasurementEstimator.ipp"
