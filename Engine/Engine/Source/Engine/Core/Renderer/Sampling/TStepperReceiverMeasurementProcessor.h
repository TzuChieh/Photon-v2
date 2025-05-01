#pragma once

#include "Engine/Core/Renderer/Sampling/TReceiverMeasurementProcessor.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>

namespace ph
{

template<typename Estimation>
class TStepperReceiverMeasurementProcessor : public TReceiverMeasurementProcessor<Estimation>
{
public:
	using Parent = TReceiverMeasurementProcessor<Estimation>;

	TStepperReceiverMeasurementProcessor() = default;

	TStepperReceiverMeasurementProcessor(
		std::size_t numEstimations,
		Integrand integrand,
		std::vector<std::shared_ptr<typename Parent::FilmType>> films);

	void onBatchStart(uint64 batchNumber) override;

	void process(
		const math::Vector2D& rasterCoord,
		const Ray&            ray, 
		const math::Spectrum& quantityWeight,
		SampleFlow&           sampleFlow) override;

	void setFilmStepSize(std::size_t filmIndex, std::size_t stepSize);

private:
	std::vector<std::size_t> m_filmStepSizes;
	uint64                   m_currentBatchNumber;
};

}// end namespace ph

#include "Engine/Core/Renderer/Sampling/TStepperReceiverMeasurementProcessor.ipp"
