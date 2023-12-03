#include "Frame/Operator/JRToneMapping.h"
#include "Common/assertion.h"

#include <limits>
#include <iostream>

namespace ph
{

JRToneMapping::JRToneMapping() : 
	FrameOperator(),
	m_exposure(1.0_r)
{}

void JRToneMapping::operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* const out_dstFrame)
{
	PH_ASSERT(out_dstFrame);

	*out_dstFrame = srcFrame;
	operateLocal(*out_dstFrame, {{0, 0}, out_dstFrame->getSizePx()});
}

void JRToneMapping::operateLocal(HdrRgbFrame& frame, const TAABB2D<uint32>& region) const
{
	frame.forEachPixel(region, [this](const HdrRgbFrame::Pixel& pixel)
	{
		HdrRgbFrame::Pixel color = pixel;
		color.mulLocal(m_exposure);
		color.subLocal(0.004_r).clampLocal(0.0_r, std::numeric_limits<real>::max());

		const HdrRgbFrame::Pixel numerator   = color.mul(6.2_r).addLocal(0.5_r).mulLocal(color);
		const HdrRgbFrame::Pixel denominator = color.mul(6.2_r).addLocal(1.7_r).mulLocal(color).addLocal(0.06_r);
		color[0] = numerator[0] / denominator[0];
		color[1] = numerator[1] / denominator[1];
		color[2] = numerator[2] / denominator[2];

		return color;
	});
}

void JRToneMapping::setExposure(const real exposure)
{
	PH_ASSERT(exposure > 0.0_r);

	m_exposure = exposure;
}

}// end namespace ph
