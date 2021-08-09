#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Common/assertion.h"
#include "Core/Filmic/SampleFilters.h"
#include "Core/Estimator/BVPTEstimator.h"
#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/BVPTDLEstimator.h"

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
