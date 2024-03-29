#include "Frame/FrameProcessor.h"

#include <Common/assertion.h>

#include <iostream>

namespace ph
{

void FrameProcessor::process(
	const PipelineId   pipeline,
	const HdrRgbFrame& srcFrame,
	HdrRgbFrame* const out_dstFrame)
{
	FrameProcessingPipeline* const targetPipeline = getPipeline(pipeline);
	if(!targetPipeline)
	{
		return;
	}

	targetPipeline->process(srcFrame, out_dstFrame);
}

FrameProcessor::PipelineId FrameProcessor::addPipeline()
{
	m_pipelines.push_back(FrameProcessingPipeline());

	return m_pipelines.size() - 1;
}

FrameProcessingPipeline* FrameProcessor::getPipeline(const PipelineId pipeline)
{
	if(!checkPipelineId(pipeline))
	{
		return nullptr;
	}

	return &(m_pipelines[pipeline]);
}

const FrameProcessingPipeline* FrameProcessor::getPipeline(const PipelineId pipeline) const
{
	if(!checkPipelineId(pipeline))
	{
		return nullptr;
	}

	return &(m_pipelines[pipeline]);
}

bool FrameProcessor::checkPipelineId(const PipelineId id) const
{
	if(id >= m_pipelines.size())
	{
		std::cerr << "warning: invalid pipeline ID detected" << std::endl;
		return false;
	}
	
	return true;
}

}// end namespace ph
