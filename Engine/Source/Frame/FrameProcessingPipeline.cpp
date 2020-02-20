#include "Frame/FrameProcessingPipeline.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void FrameProcessingPipeline::process(HdrRgbFrame& frame) const
{
	for(const auto& frameOperator : m_operators)
	{
		PH_ASSERT(frameOperator != nullptr);

		frameOperator->operate(frame);
	}
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

}// end namespace ph
