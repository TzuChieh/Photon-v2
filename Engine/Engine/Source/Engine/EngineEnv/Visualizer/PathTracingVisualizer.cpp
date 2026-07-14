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

#include <utility>

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

	const auto filter = makeSampleFilter();
	auto filmSettings = getFilmSettings();
	auto filmLayers = makeFilmLayers(filmSettings, viewport, filter);

	auto renderer = std::make_unique<EqualSamplingRenderer>(
		makeEstimator(),
		viewport,
		filter,
		ctx.numWorkers(),
		getScheduler(),
		std::move(filmLayers));

	cooked.addRenderer(std::move(renderer), std::move(filmSettings));
}

std::unique_ptr<IRayEnergyEstimator> PathTracingVisualizer::makeEstimator() const
{
	std::unique_ptr<PathEnergyEstimator> estimator;
	switch(getEstimator())
	{
	case ERayEnergyEstimator::BVPT:
		estimator = std::make_unique<BVPTEstimator>();
		break;

	case ERayEnergyEstimator::BVVPT:
		estimator = std::make_unique<BVVPTEstimator>();
		break;

	case ERayEnergyEstimator::BNEEPT:
		estimator = std::make_unique<BNEEPTEstimator>();
		break;

	case ERayEnergyEstimator::BVPTDL:
		estimator = std::make_unique<BVPTDLEstimator>();
		break;

	default:
		PH_LOG(PathTracingVisualizer, Note, "no ray energy estimator unspecified, using BNEEPT");
		estimator = std::make_unique<BNEEPTEstimator>();
		break;
	}

	PTEstimatorParams params = makePTEstimatorParams();
	estimator->setPTParams(params);
	return estimator;
}

PTEstimatorParams PathTracingVisualizer::makePTEstimatorParams() const
{
	PTEstimatorParams params{};

	if(getEstimator() == ERayEnergyEstimator::BVVPT)
	{
		params.includeVolumetricEffects = true;
	}
	else
	{
		params.includeVolumetricEffects = false;
	}

	return params;
}

}// end namespace ph
