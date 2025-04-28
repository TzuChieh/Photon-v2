#include "Engine/EngineEnv/Visualizer/FrameVisualizer.h"
#include "Engine/Core/Filmic/SampleFilter.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FrameVisualizer, Visualizer);

SampleFilter FrameVisualizer::makeSampleFilter() const
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

	default:
		PH_LOG(FrameVisualizer, Note, "sample filter unspecified, using Blackman-Harris filter");
		return SampleFilter::makeBlackmanHarris();
	}

	return {};
}

}// end namespace ph
