#pragma once

#include "Engine/Core/Renderer/Sampling/IRasterRayProcessor.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Core/Estimator/estimator_fwd.h"
#include "Engine/Core/Filmic/filmic_fwd.h"
#include "Engine/Core/Estimator/TEstimationArray.h"
#include "Engine/Core/Estimator/Integrand.h"
#include "Engine/Math/Geometry/TAABB2D.h"

#include <vector>
#include <cstddef>
#include <memory>

namespace ph
{

template<typename Estimation>
class TReceiverMeasurementProcessor : public IRasterRayProcessor
{
public:
	using FilmType      = TSamplingFilm<Estimation>;
	using EstimatorType = TIRayEstimator<Estimation>;

	TReceiverMeasurementProcessor() = default;

	TReceiverMeasurementProcessor(
		std::size_t numEstimations, 
		Integrand integrand,
		std::vector<std::shared_ptr<FilmType>> films);

	void process(
		const math::Vector2D& rasterCoord,
		const Ray&            sensedRay, 
		const math::Spectrum& quantityWeight,
		SampleFlow&           sampleFlow) override;

	/*! @brief Add an estimator to the processor.
	Use `addFilmEstimation()` to associate an estimation with a film. One estimation can also be
	associated with multiple films.
	*/
	void addEstimator(std::shared_ptr<EstimatorType> estimator);

	/*! @brief Associate an estimation with a film.
	@param filmIndex The index of the film. This will be the destination of the estimation.
	@param estimationIndex The index of the estimation (as defined in the added estimators).
	This will be the source of the estimation.
	*/
	void addFilmEstimation(std::size_t filmIndex, std::size_t estimationIndex);

	void clearFilms();
	void clearFilm(std::size_t index);
	void mergeFilmTo(std::size_t fromIndex, FilmType& toFilm);

	void setFilmDimensions(
		const math::TVector2<int64>& actualResPx,
		const math::TAABB2D<int64>& effectiveWindowPx,
		bool useSoftEdge = true);

	std::size_t numEstimations() const;
	math::TAABB2D<int64> getFilmEffectiveWindowPx() const;
	SamplingFilmDimensions getFilmDimensions() const;
	bool isSoftEdgedFilm() const;

protected:
	using EstimationToFilmMap = std::vector<std::pair<std::size_t, std::size_t>>;

	TEstimationArray<Estimation> m_estimations;
	Integrand m_integrand;
	std::vector<std::shared_ptr<FilmType>> m_films;
	std::vector<std::shared_ptr<EstimatorType>> m_estimators;
	EstimationToFilmMap m_estimationToFilm;
};

}// end namespace ph

#include "Engine/Core/Renderer/Sampling/TReceiverMeasurementProcessor.ipp"
