#include "Frame/FrameProcessingPipeline.h"
#include "Common/assertion.h"

#include <iostream>
#include <utility>

namespace ph
{

FrameProcessingPipeline::FrameProcessingPipeline(FrameProcessingPipeline&& other)
{
	*this = std::move(other);
}

void FrameProcessingPipeline::process(const HdrRgbFrame& srcFrame, HdrRgbFrame* const out_dstFrame)
{
	PH_ASSERT(out_dstFrame);

	m_tmpFrame = srcFrame;

	HdrRgbFrame* currentSrcFrame = &m_tmpFrame;
	HdrRgbFrame* currentDstFrame = out_dstFrame;

	// Ping-pong between two frames for each operator
	for(const auto& frameOperator : m_operators)
	{
		PH_ASSERT(frameOperator);

		frameOperator->operate(*currentSrcFrame, currentDstFrame);

		std::swap(currentSrcFrame, currentDstFrame);
	}

	*out_dstFrame = *currentDstFrame;
}

void FrameProcessingPipeline::appendOperator(std::unique_ptr<FrameOperator> op)
{
	if(!op)
	{
		std::cerr << "warning: empty frame operator added, ignoring" << std::endl;
		return;
	}

	m_operators.push_back(std::move(op));
}

FrameProcessingPipeline& FrameProcessingPipeline::operator = (FrameProcessingPipeline&& rhs)
{
	m_operators = std::move(rhs.m_operators);
	m_tmpFrame  = std::move(rhs.m_tmpFrame);

	return *this;
}

}// end namespace ph
