#include "Frame/HDRFrame.h"

#include <utility>

namespace ph
{

HDRFrame::HDRFrame()
{

}

HDRFrame::HDRFrame(const uint32 widthPx, const uint32 heightPx) :
	m_widthPx(widthPx), m_heightPx(heightPx), m_pixelData(static_cast<std::size_t>(3) * widthPx * heightPx, 0.0f)
{

}

HDRFrame::HDRFrame(const HDRFrame& other) : 
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx), m_pixelData(other.m_pixelData)
{

}

HDRFrame::HDRFrame(HDRFrame&& other) : 
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx), m_pixelData(std::move(other.m_pixelData))
{

}

HDRFrame::~HDRFrame()
{

}

void HDRFrame::resize(const uint32 newWidthPx, const uint32 newHeightPx)
{
	m_widthPx = newWidthPx;
	m_heightPx = newHeightPx;
	m_pixelData = std::vector<float32>(static_cast<std::size_t>(3) * newWidthPx * newHeightPx, 0.0f);
}

void HDRFrame::setPixel(const uint32 x, const uint32 y, const float32 r, const float32 g, const float32 b)
{
	const std::size_t baseIndex = (y * static_cast<std::size_t>(getWidthPx()) + x) * 3;
	m_pixelData[baseIndex + 0] = r;
	m_pixelData[baseIndex + 1] = g;
	m_pixelData[baseIndex + 2] = b;
}

HDRFrame& HDRFrame::operator = (const HDRFrame& rhs)
{
	if(this != &rhs)
	{
		m_widthPx = rhs.m_widthPx;
		m_heightPx = rhs.m_heightPx;
		m_pixelData = rhs.m_pixelData;
	}
	
	return *this;
}

HDRFrame& HDRFrame::operator = (HDRFrame&& rhs)
{
	if(this != &rhs)
	{
		m_widthPx = rhs.m_widthPx;
		m_heightPx = rhs.m_heightPx;
		m_pixelData = std::move(rhs.m_pixelData);
	}
	
	return *this;
}

}// end namespace ph