#pragma once

#include "Frame/TFrame.h"
#include "Math/TVector3.h"

namespace ph
{

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline TFrame<ComponentType, NUM_COMPONENTS>::TFrame() :
	TFrame(0, 0)
{

}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline TFrame<ComponentType, NUM_COMPONENTS>::TFrame(const uint32 wPx, const uint32 hPx) :
	m_widthPx(wPx), m_heightPx(hPx),
	m_pixelData(wPx * hPx * NUM_COMPONENTS, 0)
{

}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline TFrame<ComponentType, NUM_COMPONENTS>::TFrame(const TFrame& other) :
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_pixelData(other.m_pixelData)
{

}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline TFrame<ComponentType, NUM_COMPONENTS>::TFrame(TFrame&& other) :
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_pixelData(std::move(other.m_pixelData))
{

}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline auto TFrame<ComponentType, NUM_COMPONENTS>::getPixel(
	const uint32 x, const uint32 y, 
	Pixel* const out_pixel) const
	-> void
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	for(std::size_t i = 0; i < NUM_COMPONENTS; ++i)
	{
		(*out_pixel)[i] = m_pixelData[baseIndex + i];
	}
}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline auto TFrame<ComponentType, NUM_COMPONENTS>::setPixel(
	const uint32 x, const uint32 y, const Pixel& pixel)
	-> void
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	for(std::size_t i = 0; i < NUM_COMPONENTS; ++i)
	{
		m_pixelData[baseIndex + i] = pixel[i];
	}
}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline auto TFrame<ComponentType, NUM_COMPONENTS>::getPixelData() const
	-> const ComponentType*
{
	return m_pixelData.data();
}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline auto TFrame<ComponentType, NUM_COMPONENTS>::calcPixelDataBaseIndex(
	const uint32 x, const uint32 y) const
	-> std::size_t
{
	return (y * static_cast<std::size_t>(m_widthPx) + x) * NUM_COMPONENTS;
}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline auto TFrame<ComponentType, NUM_COMPONENTS>::operator = (const TFrame& rhs)
	-> TFrame<ComponentType, NUM_COMPONENTS>&
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = rhs.m_pixelData;

	return *this;
}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline auto TFrame<ComponentType, NUM_COMPONENTS>::operator = (TFrame&& rhs)
	-> TFrame<ComponentType, NUM_COMPONENTS>&
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = std::move(rhs.m_pixelData);

	return *this;
}

template<typename ComponentType, std::size_t NUM_COMPONENTS>
inline void swap(
	TFrame<ComponentType, NUM_COMPONENTS>& first, 
	TFrame<ComponentType, NUM_COMPONENTS>& second)
{
	using std::swap;

	swap(first.m_widthPx,   second.m_widthPx);
	swap(first.m_heightPx,  second.m_heightPx);
	swap(first.m_pixelData, second.m_pixelData);
}

}// end namespace ph