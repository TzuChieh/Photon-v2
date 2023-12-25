#include "EngineEnv/Visualizer/PhotonMappingVisualizer.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/Filmic/SampleFilter.h"
#include "Frame/Viewport.h"
#include "Core/Renderer/PM/VanillaPMRenderer.h"
#include "Core/Renderer/PM/ProgressivePMRenderer.h"
#include "Core/Renderer/PM/StochasticProgressivePMRenderer.h"
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
	switch(m_mode)
	{
	case EPhotonMappingMode::Vanilla:
	{
		renderer = std::make_unique<VanillaPMRenderer>(
			makeCommonParams(),
			viewport,
			makeSampleFilter(),
			ctx.numWorkers());
		break;
	}

	case EPhotonMappingMode::Progressive:
	{
		renderer = std::make_unique<ProgressivePMRenderer>(
			makeCommonParams(),
			viewport,
			makeSampleFilter(),
			ctx.numWorkers());
		break;
	}

	case EPhotonMappingMode::StochasticProgressive:
	{
		renderer = std::make_unique<StochasticProgressivePMRenderer>(
			makeCommonParams(),
			viewport,
			makeSampleFilter(),
			ctx.numWorkers());
		break;
	}

	default:
	{
		PH_LOG_WARNING(PhotonMappingVisualizer,
			"Unsupported PM mode ({}), no renderer generated.", TSdlEnum<EPhotonMappingMode>{}[m_mode]);
		break;
	}
	}

	cooked.addRenderer(std::move(renderer));
}

SampleFilter PhotonMappingVisualizer::makeSampleFilter() const
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

	PH_LOG(PhotonMappingVisualizer, "sample filter unspecified, using Blackman-Harris filter");
	return SampleFilter::makeBlackmanHarris();
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
