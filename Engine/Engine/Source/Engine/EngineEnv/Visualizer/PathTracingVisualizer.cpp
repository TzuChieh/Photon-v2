#include "Engine/EngineEnv/Visualizer/PathTracingVisualizer.h"
#include "Engine/EngineEnv/CoreCookingContext.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"
#include "Engine/Core/Estimator/BVPTEstimator.h"
#include "Engine/Core/Estimator/BNEEPTEstimator.h"
#include "Engine/Core/Estimator/BVPTDLEstimator.h"
#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Core/Renderer/Sampling/EqualSamplingRenderer.h"
#include "Engine/Frame/Viewport.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PathTracingVisualizer, Visualizer);

void PathTracingVisualizer::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	Viewport viewport(ctx.getFrameSizePx());

	const auto cropWindowPx = getCropWindowPx();
	if(cropWindowPx.has_value())
	{
		viewport = Viewport(ctx.getFrameSizePx(), *cropWindowPx);
	}

	auto renderer = std::make_unique<EqualSamplingRenderer>(
		makeEstimator(),
		viewport,
		makeSampleFilter(),
		ctx.numWorkers(),
		getScheduler());

	cooked.addRenderer(std::move(renderer));
}

std::unique_ptr<IRayEnergyEstimator> PathTracingVisualizer::makeEstimator() const
{
	switch(getEstimator())
	{
	case ERayEnergyEstimator::BVPT:
		return std::make_unique<BVPTEstimator>();

	case ERayEnergyEstimator::BNEEPT:
		return std::make_unique<BNEEPTEstimator>();

	case ERayEnergyEstimator::BVPTDL:
		return std::make_unique<BVPTDLEstimator>();

	default:
		PH_LOG(PathTracingVisualizer, Note, "no ray energy estimator unspecified, using BNEEPT");
		return std::make_unique<BNEEPTEstimator>();
	}

	return {};
}

}// end namespace ph
