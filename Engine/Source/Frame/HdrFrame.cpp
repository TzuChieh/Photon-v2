#include "Frame/HdrFrame.h"

namespace ph
{

HdrFrame::HdrFrame(const uint32 widthPx, const uint32 heightPx) :
	Frame(widthPx, heightPx), m_pixelData(widthPx * heightPx, 0.0f)
{

}

HdrFrame::~HdrFrame()
{

}

}// end namespace ph