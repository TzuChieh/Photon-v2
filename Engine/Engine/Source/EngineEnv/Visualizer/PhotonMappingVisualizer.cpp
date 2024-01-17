#include "EngineEnv/Visualizer/PhotonMappingVisualizer.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/Filmic/SampleFilter.h"
#include "Frame/Viewport.h"
#include "Core/Renderer/PM/VanillaPMRenderer.h"
#include "Core/Renderer/PM/ProgressivePMRenderer.h"
#include "Core/Renderer/PM/StochasticProgressivePMRenderer.h"
#include "Core/Renderer/PM/ProbabilisticProgressivePMRenderer.h"

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
		SampleFilter filter;
		if(getSampleFilter() == ESampleFilter::Unspecified)
		{
			filter = SampleFilter::makeBox();
		}
		else
		{
			if(getSampleFilter() != ESampleFilter::Box)
			{
				PH_LOG_WARNING(PhotonMappingVisualizer,
					"SPPM renderer follows Hachisuka's original paper (2009) strictly, and divides "
					"raster space into regions for storing photon statistics. Using a non-box filter "
					"will result in an overly blurred image as the stored statistics are equivalent "
					"to applying a box filter already. Increasing samples per pixel with non-box "
					"filter will approach the correct, per-sample filtered PT result in the limit.");
			}

			filter = makeSampleFilter();
		}

		renderer = std::make_unique<StochasticProgressivePMRenderer>(
			makeCommonParams(),
			viewport,
			filter,
			ctx.numWorkers());
		break;
	}

	case EPhotonMappingMode::ProbabilisticProgressive:
	{
		renderer = std::make_unique<ProbabilisticProgressivePMRenderer>(
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
