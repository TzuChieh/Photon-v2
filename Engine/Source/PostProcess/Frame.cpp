#include "PostProcess/Frame.h"
#include "Math/TVector3.h"

namespace ph
{

Frame::Frame() :
	Frame(0, 0)
{

}

Frame::Frame(const uint32 wPx, const uint32 hPx) : 
	m_widthPx(wPx), m_heightPx(hPx),
	m_rgbData(static_cast<std::size_t>(wPx) * hPx * 3, 0.0f)
{

}

Frame::Frame(const Frame& other) : 
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_rgbData(other.m_rgbData)
{

}

Frame::Frame(Frame&& other) : 
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_rgbData(std::move(other.m_rgbData))
{

}

void Frame::getRgb(const uint32 x, const uint32 y, TVector3<float32>* const out_rgb) const
{
	const std::size_t baseIndex = calcRgbDataBaseIndex(x, y);

	out_rgb->x = m_rgbData[baseIndex + 0];
	out_rgb->y = m_rgbData[baseIndex + 1];
	out_rgb->z = m_rgbData[baseIndex + 2];
}

void Frame::setRgb(const uint32 x, const uint32 y, const TVector3<float32>& rgb)
{
	const std::size_t baseIndex = calcRgbDataBaseIndex(x, y);

	m_rgbData[baseIndex + 0] = rgb.x;
	m_rgbData[baseIndex + 1] = rgb.y;
	m_rgbData[baseIndex + 2] = rgb.z;
}

const float32* Frame::getRgbData() const
{
	return m_rgbData.data();
}

std::size_t Frame::calcRgbDataBaseIndex(const uint32 x, const uint32 y) const
{
	return (y * static_cast<std::size_t>(m_widthPx) + x) * 3;
}

Frame& Frame::operator = (Frame rhs)
{
	swap(*this, rhs);

	return *this;
}

Frame& Frame::operator = (Frame&& rhs)
{
	m_widthPx  = rhs.m_widthPx;
	m_heightPx = rhs.m_heightPx;
	m_rgbData  = std::move(rhs.m_rgbData);

	return *this;
}

void swap(Frame& first, Frame& second)
{
	using std::swap;

	swap(first.m_widthPx,  second.m_widthPx);
	swap(first.m_heightPx, second.m_heightPx);
	swap(first.m_rgbData,  second.m_rgbData);
}

}// end namespace ph