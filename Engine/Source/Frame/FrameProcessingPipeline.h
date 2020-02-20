#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Frame/TFrame.h"

#include <vector>
#include <memory>


namespace ph
{

class FrameProcessingPipeline
{
public:
	void process(HdrRgbFrame& frame) const;

	void appendOperator(std::unique_ptr<FrameOperator> op);

private:
	std::vector<std::unique_ptr<FrameOperator>> m_operators;
};

}// end namespace ph
