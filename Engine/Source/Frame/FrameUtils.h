#pragma once

#include "Frame/TFrame.h"

namespace ph
{

class FrameUtils final
{
public:
	// Converts HDR frame to LDR frame. Values outside [0, 1] will be clamped.
	static void toLdr(const HdrRgbFrame& srcFrame, LdrRgbFrame* out_dstFrame);

	// Converts LDR frame to HDR frame. Values will be in [0, 1].
	static void toHdr(const LdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame);
};

}// end namespace ph