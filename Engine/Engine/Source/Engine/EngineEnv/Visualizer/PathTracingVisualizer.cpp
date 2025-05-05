#include "Engine/EngineEnv/Visualizer/PathTracingVisualizer.h"
#include "Engine/EngineEnv/CoreCookingContext.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"
#include "Engine/Core/Estimator/BVPTEstimator.h"
#include "Engine/Core/Estimator/BVVPTEstimator.h"
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
	PTEstimatorParams params = makePTEstimatorParams();

	switch(getEstimator())
	{
	case ERayEnergyEstimator::BVPT:
		return std::make_unique<BVPTEstimator>(params);

	case ERayEnergyEstimator::BVVPT:
		return std::make_unique<BVVPTEstimator>(params);

	case ERayEnergyEstimator::BNEEPT:
		return std::make_unique<BNEEPTEstimator>(params);

	case ERayEnergyEstimator::BVPTDL:
		return std::make_unique<BVPTDLEstimator>(params);

	default:
		PH_LOG(PathTracingVisualizer, Note, "no ray energy estimator unspecified, using BNEEPT");
		return std::make_unique<BNEEPTEstimator>(params);
	}

	return {};
}

PTEstimatorParams PathTracingVisualizer::makePTEstimatorParams() const
{
	PTEstimatorParams params{};

	if(getEstimator() == ERayEnergyEstimator::BVVPT)
	{
		params.includeVolumetricEffects = false;
	}
	else
	{
		params.includeVolumetricEffects = true;
	}

	return params;
}

}// end namespace ph
