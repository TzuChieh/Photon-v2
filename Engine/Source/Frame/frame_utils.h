#pragma once

#include "Frame/TFrame.h"

#include <cmath>

namespace ph
{

namespace frame_utils
{

// Converts HDR frame to LDR frame. Values outside [0, 1] will be clamped.
void to_LDR(const HdrRgbFrame& srcFrame, LdrRgbFrame* out_dstFrame);

// Converts LDR frame to HDR frame. Values will be in [0, 1].
void to_HDR(const LdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame);

// Computes per-pixel absolute difference between frameA and frameB.
void abs_diff(const HdrRgbFrame& frameA, const HdrRgbFrame& frameB, HdrRgbFrame* out_result);

// Calculates Mean Squared Error (MSE) of an estimated frame.
real calc_MSE(const HdrRgbFrame& expected, const HdrRgbFrame& estimated);

// Calculates Root-Mean-Square Error (RMSE) of an estimated frame.
real calc_RMSE(const HdrRgbFrame& expected, const HdrRgbFrame& estimated);

// In-header Implementations:

inline real calc_RMSE(const HdrRgbFrame& expected, const HdrRgbFrame& estimated)
{
	return std::sqrt(calc_MSE(expected, estimated));
}

}// end namespace frame_utils

}// end namespace ph
