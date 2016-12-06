#include "Image/PixelTexture.h"

#include <iostream>

namespace ph
{

PixelTexture::PixelTexture() :
	PixelTexture(0, 0, 0)
{

}

PixelTexture::PixelTexture(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents) :
	m_widthPx(widthPx), m_heightPx(heightPx), m_nPxComponents(nPxComponents),
	m_pixelData(static_cast<std::size_t>(nPxComponents) * widthPx * heightPx, 0.0f)
{
	if(nPxComponents > 3)
	{
		std::cerr << "warning: at PixelTexture ctor, pixel with > 3 components is not supported" << std::endl;
	}
}

PixelTexture::~PixelTexture() = default;

void PixelTexture::sample(const Vector3f& uvw, Vector3f* const out_value) const
{

}

void PixelTexture::reset(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents)
{
	m_widthPx       = widthPx;
	m_heightPx      = heightPx;
	m_nPxComponents = nPxComponents;
	m_pixelData     = std::vector<float32>(static_cast<std::size_t>(nPxComponents) * widthPx * heightPx, 0.0f);
}

void PixelTexture::setPixels(const uint32 x, const uint32 y, const uint32 widthPx, const uint32 heighPx, const uint32 nPxComponents,
                             const float32* const pixelData)
{
	if((x + widthPx > m_widthPx) || (y + heighPx > m_heightPx || (nPxComponents > m_nPxComponents)))
	{
		std::cerr << "warning: at PixelTexture::setPixels(), input overflow texture storage" << std::endl;
		return;
	}

	std::size_t dataIndex = 0;
	for(uint32 yi = y; yi < y + heighPx; yi++)
	{
		for(uint32 xi = x; xi < x + widthPx; xi++)
		{
			const uint32 baseIndex = (yi * m_widthPx) * m_nPxComponents + xi;
			for(uint32 ci = 0; ci < nPxComponents; ci++)
			{
				m_pixelData[baseIndex + ci] = pixelData[dataIndex++];
			}
		}
	}
}

}// end namespace ph