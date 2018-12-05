#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Common/primitive_type.h"
#include "Core/Quantity/ColorSpace.h"

#include <cmath>

namespace ph
{

class NaiveReinhardToneMapping : public FrameOperator
{
public:
	NaiveReinhardToneMapping();

	void operate(HdrRgbFrame& frame) const override;
};

// In-header Implementations:

inline NaiveReinhardToneMapping::NaiveReinhardToneMapping()
{}

inline void NaiveReinhardToneMapping::operate(HdrRgbFrame& frame) const
{
	frame.forEachPixel([this](const HdrRgbFrame::Pixel& pixel)
	{
		return pixel.div(pixel + 1.0_r);
	});
}

}// end namespace ph