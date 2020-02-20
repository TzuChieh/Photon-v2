#pragma once

#include "Frame/TFrame.h"

namespace ph
{

class FrameOperator
{
public:
	virtual ~FrameOperator();

	virtual void operate(HdrRgbFrame& frame) const = 0;
};

}// end namespace ph
