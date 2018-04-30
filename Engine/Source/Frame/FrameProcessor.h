#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Frame/TFrame.h"

#include <vector>
#include <memory>

namespace ph
{

class FrameProcessor final 
{
public:
	void process(HdrRgbFrame& frame) const;

	void appendOperator(const std::shared_ptr<FrameOperator>& op);

private:
	std::vector<std::shared_ptr<FrameOperator>> m_operators;
};

}// end namespace ph