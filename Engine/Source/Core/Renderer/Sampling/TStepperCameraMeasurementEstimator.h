#pragma once

#include "Core/Renderer/Sampling/TCameraMeasurementEstimator.h"
#include "Core/Filmic/HdrRgbFilm.h"

#include <cstddef>
#include <vector>

namespace ph
{

template<typename SamplingFilmType, typename EstimationType>
class TStepperCameraMeasurementEstimator : public TCameraMeasurementEstimator<SamplingFilmType, EstimationType>
{
public:
	using Parent = TCameraMeasurementEstimator<SamplingFilmType, EstimationType>;

	TStepperCameraMeasurementEstimator() = default;
	TStepperCameraMeasurementEstimator(
		std::size_t  numFilms,
		std::size_t  numEstimations,
		Integrand    integrand,
		SampleFilter filter);
	TStepperCameraMeasurementEstimator(TStepperCameraMeasurementEstimator&& other) = default;

	void onBatchStart(uint64 batchNumber) override;
	void process(const Vector2D& filmNdc, const Ray& ray) override;

	void setFilmStepSize(std::size_t filmIndex, std::size_t stepSize);

	TStepperCameraMeasurementEstimator& operator = (TStepperCameraMeasurementEstimator&& other) = default;

private:
	std::vector<std::size_t> m_filmStepSizes;
	uint64                   m_currentBatchNumber;
};

}// end namespace ph

#include "Core/Renderer/Sampling/TStepperCameraMeasurementEstimator.ipp"