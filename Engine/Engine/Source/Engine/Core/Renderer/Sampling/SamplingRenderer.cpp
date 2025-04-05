#include "Engine/Core/Renderer/Sampling/SamplingRenderer.h"
#include "Engine/Core/Estimator/BVPTEstimator.h"
#include "Engine/Core/Estimator/BNEEPTEstimator.h"
#include "Engine/Core/Estimator/BVPTDLEstimator.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

SamplingRenderer::SamplingRenderer(
	std::unique_ptr<IRayEnergyEstimator> estimator,
	Viewport                             viewport,
	SampleFilter                         filter,
	const uint32                         numWorkers) : 

	Renderer(std::move(viewport), numWorkers),

	m_estimator(std::move(estimator)),
	m_filter   (std::move(filter))
{
	PH_ASSERT(m_estimator);
}

}// end namespace ph
