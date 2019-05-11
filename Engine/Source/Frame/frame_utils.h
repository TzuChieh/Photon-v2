#pragma once

#include "Frame/TFrame.h"

namespace ph
{

namespace frame_utils
{

// Converts HDR frame to LDR frame. Values outside [0, 1] will be clamped.
void to_LDR(const HdrRgbFrame& srcFrame, LdrRgbFrame* out_dstFrame);

// Converts LDR frame to HDR frame. Values will be in [0, 1].
void to_HDR(const LdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame);

}// end namespace frame_utils

}// end namespace ph