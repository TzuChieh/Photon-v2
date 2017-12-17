#pragma once

#include "Frame/TFrame.h"
#include "Math/TVector3.h"

namespace ph
{

template<typename ComponentType>
inline TFrame<ComponentType>::TFrame() :
	TFrame(0, 0)
{

}

template<typename ComponentType>
inline TFrame<ComponentType>::TFrame(const uint32 wPx, const uint32 hPx) :
	m_widthPx(wPx), m_heightPx(hPx),
	m_pixelData(static_cast<std::size_t>(wPx) * hPx * 3, 0)
{

}

template<typename ComponentType>
inline TFrame<ComponentType>::TFrame(const TFrame& other) :
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_pixelData(other.m_pixelData)
{

}

template<typename ComponentType>
inline TFrame<ComponentType>::TFrame(TFrame&& other) :
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_pixelData(std::move(other.m_pixelData))
{

}

template<typename ComponentType>
inline void TFrame<ComponentType>::getPixel(
	const uint32 x, const uint32 y, 
	TVector3<ComponentType>* const out_pixel) const
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	out_pixel->x = m_pixelData[baseIndex + 0];
	out_pixel->y = m_pixelData[baseIndex + 1];
	out_pixel->z = m_pixelData[baseIndex + 2];
}

template<typename ComponentType>
inline void TFrame<ComponentType>::setPixel(
	const uint32 x, const uint32 y, const TVector3<ComponentType>& pixel)
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	m_pixelData[baseIndex + 0] = pixel.x;
	m_pixelData[baseIndex + 1] = pixel.y;
	m_pixelData[baseIndex + 2] = pixel.z;
}

template<typename ComponentType>
inline const ComponentType* TFrame<ComponentType>::getPixelData() const
{
	return m_pixelData.data();
}

template<typename ComponentType>
inline std::size_t TFrame<ComponentType>::calcPixelDataBaseIndex(
	const uint32 x, const uint32 y) const
{
	return (y * static_cast<std::size_t>(m_widthPx) + x) * 3;
}

template<typename ComponentType>
inline TFrame<ComponentType>& TFrame<ComponentType>::operator = (const TFrame& rhs)
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = rhs.m_pixelData;

	return *this;
}

template<typename ComponentType>
inline TFrame<ComponentType>& TFrame<ComponentType>::operator = (TFrame&& rhs)
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = std::move(rhs.m_pixelData);

	return *this;
}

template<typename ComponentType>
inline void swap(TFrame<ComponentType>& first, TFrame<ComponentType>& second)
{
	using std::swap;

	swap(first.m_widthPx,   second.m_widthPx);
	swap(first.m_heightPx,  second.m_heightPx);
	swap(first.m_pixelData, second.m_pixelData);
}

}// end namespace ph