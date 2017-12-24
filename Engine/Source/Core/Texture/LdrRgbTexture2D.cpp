#include "Core/Texture/LdrRgbTexture2D.h"
#include "Math/Math.h"
#include "FileIO/InputPacket.h"
#include "Core/Texture/TextureLoader.h"
#include "Common/assertion.h"

#include <iostream>
#include <string>

namespace ph
{

LdrRgbTexture2D::LdrRgbTexture2D() :
	LdrRgbTexture2D(0, 0)
{

}

LdrRgbTexture2D::LdrRgbTexture2D(const uint32 widthPx, const uint32 heightPx) :
	TTexture(), 
	m_widthPx(widthPx), m_heightPx(heightPx),
	m_pixelData(static_cast<std::size_t>(widthPx) * heightPx * 3, 0.0_r)
{

}

LdrRgbTexture2D::~LdrRgbTexture2D() = default;

void LdrRgbTexture2D::sample(const Vector3R& uvw, SpectralStrength* const out_value) const
{
	PH_ASSERT(out_value != nullptr);

	const std::size_t baseIndex = calcSampleBaseIndex(uvw);
	const Vector3R rgb(m_pixelData[baseIndex + 0],
	                   m_pixelData[baseIndex + 1], 
	                   m_pixelData[baseIndex + 2]);
	out_value->setRgb(rgb);
}

void LdrRgbTexture2D::resize(const uint32 widthPx, const uint32 heightPx)
{
	m_widthPx   = widthPx;
	m_heightPx  = heightPx;
	m_pixelData = std::vector<real>(static_cast<std::size_t>(widthPx) * heightPx * 3, 0.0_r);
}

void LdrRgbTexture2D::setPixels(const uint32 x, const uint32 y, const uint32 widthPx, const uint32 heighPx,
                                const real* const pixelData)
{
	PH_ASSERT(pixelData != nullptr);

	if((x + widthPx > m_widthPx) || (y + heighPx > m_heightPx))
	{
		std::cerr << "warning: at PixelTexture::setPixels(), " 
		          << "input overflow texture storage" << std::endl;
		return;
	}

	std::size_t dataIndex = 0;
	for(uint32 yi = y; yi < y + heighPx; yi++)
	{
		for(uint32 xi = x; xi < x + widthPx; xi++)
		{
			const uint32 baseIndex = (yi * m_widthPx + xi) * 3;
			m_pixelData[baseIndex + 0] = pixelData[dataIndex++];
			m_pixelData[baseIndex + 1] = pixelData[dataIndex++];
			m_pixelData[baseIndex + 2] = pixelData[dataIndex++];
		}
	}
}

std::size_t LdrRgbTexture2D::calcSampleBaseIndex(const Vector3R& uvw) const
{
	const real u = uvw.x;
	const real v = uvw.y;

	const int32 maxX = static_cast<int32>(m_widthPx - 1);
	const int32 maxY = static_cast<int32>(m_heightPx - 1);
	int32 x = static_cast<int32>(u * static_cast<real>(maxX));
	int32 y = static_cast<int32>(v * static_cast<real>(maxY));

	x = x % maxX;
	y = y % maxY;
	x = x < 0 ? x + maxX : x;
	y = y < 0 ? y + maxY : y;

	return (y * m_widthPx + x) * 3;
}

}// end namespace ph