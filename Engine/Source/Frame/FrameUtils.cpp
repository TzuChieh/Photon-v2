#include "Frame/FrameUtils.h"
#include "Common/assertion.h"

namespace ph
{

void FrameUtils::toLdr(const HdrRgbFrame& srcFrame, LdrRgbFrame* const out_dstFrame)
{
	PH_ASSERT(out_dstFrame);

	out_dstFrame->setSize(srcFrame.getSizePx());

	srcFrame.forEachPixel(
		[out_dstFrame](const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& hdrPixel)
		{
			const HdrRgbFrame::Pixel mappedPixel = hdrPixel.mul(255.0_r).add(0.5_r).clampLocal(0.0_r, 255.0_r);
			out_dstFrame->setPixel(x, y, LdrRgbFrame::Pixel(mappedPixel));
		});
}

void FrameUtils::toHdr(const LdrRgbFrame& srcFrame, HdrRgbFrame* const out_dstFrame)
{
	PH_ASSERT(out_dstFrame);

	out_dstFrame->setSize(srcFrame.getSizePx());

	srcFrame.forEachPixel(
		[out_dstFrame](const uint32 x, const uint32 y, const LdrRgbFrame::Pixel& ldrPixel)
		{
			out_dstFrame->setPixel(
				x, 
				y, 
				HdrRgbFrame::Pixel(ldrPixel).div(255.0_r));
		});
}

}// end namespace ph