#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Common/primitive_type.h"
#include "Core/Quantity/ColorSpace.h"

#include <cmath>

namespace ph
{

class GammaCorrection : public FrameOperator
{
public:
	explicit GammaCorrection(real gamma);

	void operate(HdrRgbFrame& frame) const override;

	void useSrgbStandard(bool value);

private:
	real m_reciGamma;
	bool m_useSrgbStandard;
};

// In-header Implementations:

inline GammaCorrection::GammaCorrection(const real gamma) :
	m_reciGamma(1.0_r / gamma), m_useSrgbStandard(true)
{}

inline void GammaCorrection::operate(HdrRgbFrame& frame) const
{
	if(m_useSrgbStandard)
	{
		frame.forEachPixel([this](const HdrRgbFrame::Pixel& pixel)
		{
			const Vector3R linearSrgb(pixel[0], pixel[1], pixel[2]);
			const Vector3R srgb = ColorSpace::linear_sRGB_to_sRGB(linearSrgb);

			return HdrRgbFrame::Pixel({srgb.x, srgb.y, srgb.z});
		});
	}
	else
	{
		frame.forEachPixel([this](const HdrRgbFrame::Pixel& pixel)
		{
			const Vector3R rgb(
				std::pow(pixel[0], m_reciGamma),
				std::pow(pixel[1], m_reciGamma),
				std::pow(pixel[2], m_reciGamma));

			return HdrRgbFrame::Pixel({rgb.x, rgb.y, rgb.z});
		});
	}
}

inline void GammaCorrection::useSrgbStandard(const bool value)
{
	m_useSrgbStandard = value;
}

}// end namespace ph
