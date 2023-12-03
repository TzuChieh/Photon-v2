#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Estimator/FullRayEnergyEstimator.h"

#include <memory>

namespace ph
{

class SamplingRenderer : public Renderer
{
public:
	SamplingRenderer(
		std::unique_ptr<IRayEnergyEstimator> estimator,
		Viewport                             viewport, 
		SampleFilter                         filter,
		uint32                               numWorkers);

protected:
	SampleFilter                         m_filter;
	std::unique_ptr<IRayEnergyEstimator> m_estimator;
};

}// end namespace ph
