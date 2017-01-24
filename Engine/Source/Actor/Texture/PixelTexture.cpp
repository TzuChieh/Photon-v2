#include "Actor/Texture/PixelTexture.h"
#include "Math/Math.h"
#include "FileIO/InputPacket.h"
#include "Actor/Texture/TextureLoader.h"

#include <iostream>
#include <string>

namespace ph
{

PixelTexture::PixelTexture() : 
	PixelTexture(0, 0, 0)
{

}

PixelTexture::PixelTexture(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents) : 
	Texture(), 
	m_widthPx(widthPx), m_heightPx(heightPx), m_nPxComponents(nPxComponents),
	m_pixelData(static_cast<std::size_t>(nPxComponents) * widthPx * heightPx, 0.0f)
{
	if(nPxComponents > 3)
	{
		std::cerr << "warning: at PixelTexture ctor, pixel with > 3 components is not supported" << std::endl;
	}
}

PixelTexture::PixelTexture(const InputPacket& packet) : 
	Texture(packet)
{
	const std::string filename = packet.getString("filename", "", "PixelTexture -> filename not found");
	TextureLoader().load(filename, this);
}

PixelTexture::~PixelTexture() = default;

void PixelTexture::sample(const Vector3f& uvw, Vector3f* const out_value) const
{
	const float32 u = uvw.x;
	const float32 v = uvw.y;

	const int32 maxX = static_cast<int32>(m_widthPx - 1);
	const int32 maxY = static_cast<int32>(m_heightPx - 1);
	int32 x = static_cast<int32>(u * static_cast<float32>(maxX));
	int32 y = static_cast<int32>(v * static_cast<float32>(maxY));

	x = x % maxX;
	y = y % maxY;
	x = x < 0 ? x + maxX : x;
	y = y < 0 ? y + maxY : y;

	const uint32 baseIndex = (y * m_widthPx + x) * m_nPxComponents;

	out_value->x = m_pixelData[baseIndex + 0];
	out_value->y = m_pixelData[baseIndex + 1];
	out_value->z = m_pixelData[baseIndex + 2];

	switch(m_nPxComponents)
	{
	case 3:
		out_value->z = m_pixelData[baseIndex + 2];
	case 2:
		out_value->y = m_pixelData[baseIndex + 1];
	case 1:
		out_value->x = m_pixelData[baseIndex + 0];
	}
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
			const uint32 baseIndex = (yi * m_widthPx + xi) * m_nPxComponents;
			for(uint32 ci = 0; ci < nPxComponents; ci++)
			{
				m_pixelData[baseIndex + ci] = pixelData[dataIndex++];
			}
		}
	}
}

}// end namespace ph