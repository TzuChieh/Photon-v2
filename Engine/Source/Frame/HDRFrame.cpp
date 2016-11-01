#include "Frame/HDRFrame.h"

#include <utility>

namespace ph
{

HDRFrame::HDRFrame()
{

}

HDRFrame::HDRFrame(const uint32 widthPx, const uint32 heightPx) :
	m_widthPx(widthPx), m_heightPx(heightPx), m_pixelData(widthPx * heightPx, 0.0f)
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
	m_pixelData = std::vector<float32>(newWidthPx * newHeightPx, 0.0f);
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