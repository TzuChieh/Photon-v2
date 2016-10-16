#pragma once

#include "Common/primitive_type.h"
#include "Frame/Frame.h"

#include <vector>

namespace ph
{

class HdrFrame : public Frame
{
public:
	HdrFrame(const uint32 widthPx, const uint32 heightPx);
	virtual ~HdrFrame() override;

private:
	std::vector<float32> m_pixelData;
};

}// end namespace ph