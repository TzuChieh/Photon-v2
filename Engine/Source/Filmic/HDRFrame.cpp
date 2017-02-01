#include "Filmic/HDRFrame.h"
#include "Math/TVector3.h"

#include <utility>

namespace ph
{

HDRFrame::HDRFrame()
{

}

HDRFrame::HDRFrame(const uint32 widthPx, const uint32 heightPx) :
	m_widthPx(widthPx), m_heightPx(heightPx), m_pixelData(static_cast<std::size_t>(3) * widthPx * heightPx, 0.0_r)
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
	m_pixelData = std::vector<real>(static_cast<std::size_t>(3) * newWidthPx * newHeightPx, 0.0_r);
}

void HDRFrame::getPixel(const uint32 x, const uint32 y, Vector3R* const out_pixel) const
{
	const std::size_t baseIndex = (y * static_cast<std::size_t>(getWidthPx()) + x) * 3;

	out_pixel->x = m_pixelData[baseIndex + 0];
	out_pixel->y = m_pixelData[baseIndex + 1];
	out_pixel->z = m_pixelData[baseIndex + 2];
}

void HDRFrame::setPixel(const uint32 x, const uint32 y, const real r, const real g, const real b)
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