#include "Frame/HdrRectFrame.h"

namespace ph
{

HdrRectFrame::HdrRectFrame(const uint32 widthPx, const uint32 heightPx) : 
	m_widthPx(widthPx), m_heightPx(heightPx), m_pixelData(widthPx * heightPx, 0.0f)
{

}

HdrRectFrame::~HdrRectFrame()
{

}

}// end namespace ph