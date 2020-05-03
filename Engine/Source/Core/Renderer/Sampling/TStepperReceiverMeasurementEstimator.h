#pragma once

#include "Core/Renderer/Sampling/TReceiverMeasurementEstimator.h"
#include "Core/Filmic/HdrRgbFilm.h"

#include <cstddef>
#include <vector>

namespace ph
{

template<typename SamplingFilmType, typename EstimationType>
class TStepperReceiverMeasurementEstimator : public TReceiverMeasurementEstimator<SamplingFilmType, EstimationType>
{
public:
	using Parent = TReceiverMeasurementEstimator<SamplingFilmType, EstimationType>;

	TStepperReceiverMeasurementEstimator() = default;
	TStepperReceiverMeasurementEstimator(
		std::size_t  numFilms,
		std::size_t  numEstimations,
		Integrand    integrand,
		SampleFilter filter);
	TStepperReceiverMeasurementEstimator(TStepperReceiverMeasurementEstimator&& other) = default;

	void onBatchStart(uint64 batchNumber) override;

	void process(
		const math::Vector2D& rasterCoord,
		const Ray&            ray, 
		const Spectrum&       quantityWeight,
		SampleFlow&           sampleFlow) override;

	void setFilmStepSize(std::size_t filmIndex, std::size_t stepSize);

	TStepperReceiverMeasurementEstimator& operator = (TStepperReceiverMeasurementEstimator&& other) = default;

private:
	std::vector<std::size_t> m_filmStepSizes;
	uint64                   m_currentBatchNumber;
};

}// end namespace ph

#include "Core/Renderer/Sampling/TStepperReceiverMeasurementEstimator.ipp"
