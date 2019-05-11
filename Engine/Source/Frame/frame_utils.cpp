#include "Frame/frame_utils.h"
#include "Common/assertion.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("Frame Utils"));
}

namespace frame_utils
{

void to_LDR(const HdrRgbFrame& srcFrame, LdrRgbFrame* const out_dstFrame)
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

void to_HDR(const LdrRgbFrame& srcFrame, HdrRgbFrame* const out_dstFrame)
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

real calc_MSE(const HdrRgbFrame& expected, const HdrRgbFrame& estimated)
{
	PH_ASSERT_GT(expected.getSizePx().product(),  0);
	PH_ASSERT_GT(estimated.getSizePx().product(), 0);

	if(!expected.getSizePx().equals(estimated.getSizePx()))
	{
		logger.log(ELogLevel::WARNING_MED, 
			"Dimension mismatch in calc_MSE(2); "
			"sizes are: " + expected.getSizePx().toString() + ", " +
			                estimated.getSizePx().toString());
		return 0.0_r;
	}

	double MSE = 0.0;
	expected.forEachPixel(
		[&estimated, &MSE]
		(const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& expectedPixel)
		{
			const auto estimatedPixel = estimated.getPixel({x, y});

			const auto diff = expectedPixel.sub(estimatedPixel);

			MSE += diff[0] * diff[0];
			MSE += diff[1] * diff[1];
			MSE += diff[2] * diff[2];
		});
	MSE /= static_cast<double>(expected.getSizePx().product() * 3);

	PH_ASSERT_GE(MSE, 0.0_r);
	return MSE;
}

}// end namespace frame_utils

}// end namespace ph