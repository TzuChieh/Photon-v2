#pragma once

#include "Common/primitive_type.h"
#include "Frame/Frame.h"

#include <vector>

namespace ph
{

class HdrRectFrame : public Frame
{
public:
	HdrRectFrame(const uint32 widthPx, const uint32 heightPx);
	virtual ~HdrRectFrame() override;

private:
	const uint32 m_widthPx;
	const uint32 m_heightPx;

	std::vector<float32> m_pixelData;
};

}// end namespace ph