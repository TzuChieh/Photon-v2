#include "EngineEnv/Visualizer/PathTracingVisualizer.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/Estimator/BVPTEstimator.h"
#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/BVPTDLEstimator.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/Sampling/EqualSamplingRenderer.h"
#include "Frame/Viewport.h"

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

SampleFilter PathTracingVisualizer::makeSampleFilter() const
{
	switch(getSampleFilter())
	{
	case ESampleFilter::Box:
		return SampleFilter::makeBox();

	case ESampleFilter::Gaussian:
		return SampleFilter::makeGaussian();

	case ESampleFilter::MitchellNetravali:
		return SampleFilter::makeMitchellNetravali();

	case ESampleFilter::BlackmanHarris:
		return SampleFilter::makeBlackmanHarris();
	}

	PH_LOG(PathTracingVisualizer, "sample filter unspecified, using Blackman-Harris filter");
	return SampleFilter::makeBlackmanHarris();
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
	}

	PH_LOG(PathTracingVisualizer, "no ray energy estimator unspecified, using BNEEPT");
	return std::make_unique<BNEEPTEstimator>();
}

}// end namespace ph
