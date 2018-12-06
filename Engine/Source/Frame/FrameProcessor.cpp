#include "Frame/FrameProcessor.h"
#include "Common/assertion.h"

namespace ph
{
void FrameProcessor::process(HdrRgbFrame& frame) const
{
	for(const auto& frameOperator : m_operators)
	{
		PH_ASSERT(frameOperator != nullptr);

		frameOperator->operate(frame);
	}
}

void FrameProcessor::appendOperator(std::unique_ptr<FrameOperator> op)
{
	if(!op)
	{
		return;
	}

	m_operators.push_back(std::move(op));
}

}// end namespace ph