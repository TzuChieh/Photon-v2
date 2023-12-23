#include "EngineEnv/Visualizer/PhotonMappingVisualizer.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/Filmic/SampleFilters.h"
#include "Frame/Viewport.h"
#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Renderer/PM/VanillaPMRenderer.h"
#include "Core/Renderer/PM/EPMMode.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PhotonMappingVisualizer, Visualizer);

void PhotonMappingVisualizer::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	Viewport viewport(ctx.getFrameSizePx());

	auto optCropWindow = getCropWindowPx();
	if(optCropWindow.has_value())
	{
		viewport = Viewport(ctx.getFrameSizePx(), *optCropWindow);
	}

	auto mode = EPMMode::VANILLA;
	switch(m_mode)
	{
	case EPhotonMappingMode::Vanilla:
		mode = EPMMode::VANILLA;
		break;

	case EPhotonMappingMode::Progressive:
		mode = EPMMode::PROGRESSIVE;
		break;

	case EPhotonMappingMode::StochasticProgressive:
		mode = EPMMode::STOCHASTIC_PROGRESSIVE;
		break;
	}

	std::unique_ptr<Renderer> renderer;
	if(m_mode == EPhotonMappingMode::Vanilla)
	{
		renderer = std::make_unique<VanillaPMRenderer>(
			makeCommonParams(),
			viewport,
			makeSampleFilter(),
			ctx.numWorkers());
	}
	else
	{
		renderer = std::make_unique<PMRenderer>(
			mode,
			makeCommonParams(),
			viewport,
			makeSampleFilter(),
			ctx.numWorkers());
	}

	cooked.addRenderer(std::move(renderer));
}

SampleFilter PhotonMappingVisualizer::makeSampleFilter() const
{
	switch(getSampleFilter())
	{
	case ESampleFilter::Box:
		return SampleFilters::createBoxFilter();

	case ESampleFilter::Gaussian:
		return SampleFilters::createGaussianFilter();

	case ESampleFilter::MitchellNetravali:
		return SampleFilters::createMitchellNetravaliFilter();

	case ESampleFilter::BlackmanHarris:
		return SampleFilters::createBlackmanHarrisFilter();
	}

	PH_LOG(PhotonMappingVisualizer, "sample filter unspecified, using Blackman-Harris filter");
	return SampleFilters::createBlackmanHarrisFilter();
}

PMCommonParams PhotonMappingVisualizer::makeCommonParams() const
{
	return PMCommonParams{
		.numPhotons = m_numPhotons,
		.numPasses = m_numPasses,
		.numSamplesPerPixel = m_numSamplesPerPixel,
		.kernelRadius = m_photonRadius};
}

}// end namespace ph
