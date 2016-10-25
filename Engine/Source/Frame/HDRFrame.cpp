#include "Frame/HDRFrame.h"

namespace ph
{

HDRFrame::HDRFrame(const uint32 widthPx, const uint32 heightPx) :
	Frame(widthPx, heightPx), m_pixelData(widthPx * heightPx, 0.0f)
{

}

HDRFrame::~HDRFrame()
{

}

}// end namespace ph