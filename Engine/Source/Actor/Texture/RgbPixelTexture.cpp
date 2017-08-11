#include "Actor/Texture/RgbPixelTexture.h"
#include "Math/Math.h"
#include "FileIO/InputPacket.h"
#include "Actor/Texture/TextureLoader.h"

#include <iostream>
#include <string>

namespace ph
{

RgbPixelTexture::RgbPixelTexture() :
	RgbPixelTexture(0, 0, 0)
{

}

RgbPixelTexture::RgbPixelTexture(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents) :
	Texture(), 
	m_widthPx(widthPx), m_heightPx(heightPx), m_nPxComponents(nPxComponents),
	m_pixelData(static_cast<std::size_t>(nPxComponents) * widthPx * heightPx, 0.0f)
{
	if(nPxComponents > 3)
	{
		std::cerr << "warning: at RgbPixelTexture::RgbPixelTexture(), " 
		          << "pixel with > 3 components is not supported" << std::endl;
	}
}

RgbPixelTexture::RgbPixelTexture(const InputPacket& packet) :
	Texture(packet)
{
	const std::string filename = packet.getString("filename", "", DataTreatment::REQUIRED());
	TextureLoader().load(filename, this);
}

RgbPixelTexture::~RgbPixelTexture() = default;

void RgbPixelTexture::sample(const Vector3R& uvw, SpectralStrength* const out_value) const
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

	const uint32 baseIndex = (y * m_widthPx + x) * m_nPxComponents;

	Vector3R rgb(0);
	switch(m_nPxComponents)
	{
	case 3:
		rgb.z = m_pixelData[baseIndex + 2];
	case 2:
		rgb.y = m_pixelData[baseIndex + 1];
	case 1:
		rgb.x = m_pixelData[baseIndex + 0];
	}

	out_value->setRgb(rgb);
}

void RgbPixelTexture::reset(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents)
{
	m_widthPx       = widthPx;
	m_heightPx      = heightPx;
	m_nPxComponents = nPxComponents;
	m_pixelData     = std::vector<real>(static_cast<std::size_t>(nPxComponents) * widthPx * heightPx, 0.0f);
}

void RgbPixelTexture::setPixels(const uint32 x, const uint32 y, const uint32 widthPx, const uint32 heighPx, const uint32 nPxComponents,
                             const real* const pixelData)
{
	if((x + widthPx > m_widthPx) || (y + heighPx > m_heightPx || (nPxComponents > m_nPxComponents)))
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
			const uint32 baseIndex = (yi * m_widthPx + xi) * m_nPxComponents;
			for(uint32 ci = 0; ci < nPxComponents; ci++)
			{
				m_pixelData[baseIndex + ci] = pixelData[dataIndex++];
			}
		}
	}
}

}// end namespace ph