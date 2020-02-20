#pragma once

#include "Frame/FrameProcessingPipeline.h"

#include <vector>
#include <memory>

namespace ph
{

// FIXME: if frame processor is used concurrently, things may break
// if some frame operators used cached data internally, need to solve
// this issue

class FrameProcessor final 
{
public:
	using PipelineId = std::size_t;

	void process(HdrRgbFrame& frame, PipelineId pipeline) const;

	PipelineId addPipeline();
	FrameProcessingPipeline* getPipeline(PipelineId pipeline);
	const FrameProcessingPipeline* getPipeline(PipelineId pipeline) const;

private:
	std::vector<FrameProcessingPipeline> m_pipelines;

	bool checkPipelineId(PipelineId id) const;
};

}// end namespace ph
