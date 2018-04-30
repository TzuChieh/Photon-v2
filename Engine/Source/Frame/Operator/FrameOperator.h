#pragma once

#include "Frame/TFrame.h"

namespace ph
{

class FrameOperator
{
public:
	virtual ~FrameOperator() = 0;

	virtual void operate(const HdrRgbFrame& input, HdrRgbFrame& output) const = 0;
};

}// end namespace ph