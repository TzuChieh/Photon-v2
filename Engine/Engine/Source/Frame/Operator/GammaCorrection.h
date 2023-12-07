#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Core/Quantity/ColorSpace.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cmath>

namespace ph
{

class GammaCorrection : public FrameOperator
{
public:
	explicit GammaCorrection(real gamma);

	void operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame) override;

	void useSrgbStandard(bool value);

private:
	real m_reciGamma;
	bool m_useSrgbStandard;
};

// In-header Implementations:

inline GammaCorrection::GammaCorrection(const real gamma) :
	m_reciGamma(1.0_r / gamma), m_useSrgbStandard(true)
{}

inline void GammaCorrection::operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* const out_dstFrame)
{
	PH_ASSERT(out_dstFrame);

	out_dstFrame->setSize(srcFrame.getSizePx());

	if(m_useSrgbStandard)
	{
		srcFrame.forEachPixel(
			[this, out_dstFrame]
			(const uint32 x, const uint32 y, const HdrRgbFrame::PixelType& srcPixel)
			{
				const math::Vector3R linearSrgb(srcPixel[0], srcPixel[1], srcPixel[2]);
				const math::Vector3R srgb = ColorSpace::linear_sRGB_to_sRGB(linearSrgb);

				out_dstFrame->setPixel(x, y, HdrRgbFrame::PixelType({srgb.x, srgb.y, srgb.z}));
			});
	}
	else
	{
		srcFrame.forEachPixel(
			[this, out_dstFrame]
			(const uint32 x, const uint32 y, const HdrRgbFrame::PixelType& pixel)
			{
				const math::Vector3R rgb(
					std::pow(pixel[0], m_reciGamma),
					std::pow(pixel[1], m_reciGamma),
					std::pow(pixel[2], m_reciGamma));

				out_dstFrame->setPixel(x, y, HdrRgbFrame::PixelType({rgb.x, rgb.y, rgb.z}));
			});
	}
}

inline void GammaCorrection::useSrgbStandard(const bool value)
{
	m_useSrgbStandard = value;
}

}// end namespace ph
