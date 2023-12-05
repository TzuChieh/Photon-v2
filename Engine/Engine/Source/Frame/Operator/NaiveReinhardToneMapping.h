#pragma once

#include "Frame/Operator/FrameOperator.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cmath>

namespace ph
{

class NaiveReinhardToneMapping : public FrameOperator
{
public:
	NaiveReinhardToneMapping();

	void operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame) override;
};

// In-header Implementations:

inline NaiveReinhardToneMapping::NaiveReinhardToneMapping()
{}

inline void NaiveReinhardToneMapping::operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* const out_dstFrame)
{
	PH_ASSERT(out_dstFrame);

	out_dstFrame->setSize(srcFrame.getSizePx());

	srcFrame.forEachPixel(
		[this, out_dstFrame]
		(const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& pixel)
		{
			out_dstFrame->setPixel(x, y, pixel.div(pixel + 1.0_r));
		});
}

}// end namespace ph
