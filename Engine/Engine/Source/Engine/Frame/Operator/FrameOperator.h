#pragma once

#include "Engine/Frame/TFrame.h"

namespace ph
{

class FrameOperator
{
public:
	virtual inline ~FrameOperator() = default;

	virtual void operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame) = 0;
};

}// end namespace ph
