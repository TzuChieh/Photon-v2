#pragma once

#include "Core/Texture/Pixel/TPixelTexture2D.h"
#include "Common/assertion.h"
#include "Math/TArithmeticArray.h"

#include <stdexcept>
#include <cmath>

namespace ph
{

template<typename OutputType>
inline TPixelTexture2D<OutputType>::TPixelTexture2D(const std::shared_ptr<PixelBuffer2D>& pixelBuffer) :
	TPixelTexture2D(
		pixelBuffer,
		pixel_texture::ESampleMode::Bilinear,
		pixel_texture::EWrapMode::ClampToEdge)
{}

template<typename OutputType>
inline TPixelTexture2D<OutputType>::TPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapModeS,
	const pixel_texture::EWrapMode        wrapModeT) :

	TTexture<OutputType>(),

	m_pixelBuffer(pixelBuffer),
	m_sampleMode (sampleMode),
	m_wrapModeS  (wrapModeS),
	m_wrapModeT  (wrapModeT),
	m_texelSize  (0)
{
	if(!m_pixelBuffer)
	{
		throw std::invalid_argument(
			"Input pixel buffer is empty.");
	}

	m_texelSize = math::Vector2D(1.0) / math::Vector2D(m_pixelBuffer->getSize());
}

template<typename OutputType>
inline math::TVector2<uint32> TPixelTexture2D<OutputType>::getSizePx() const
{
	PH_ASSERT(m_pixelBuffer);
	return m_pixelBuffer->getSize();
}

template<typename OutputType>
inline math::Vector2D TPixelTexture2D<OutputType>::getTexelSize() const
{
	return m_texelSize;
}

template<typename OutputType>
inline pixel_texture::ESampleMode TPixelTexture2D<OutputType>::getSampleMode() const
{
	return m_sampleMode;
}

template<typename OutputType>
inline pixel_texture::EWrapMode TPixelTexture2D<OutputType>::getWrapModeS() const
{
	return m_wrapModeS;
}

template<typename OutputType>
inline pixel_texture::EWrapMode TPixelTexture2D<OutputType>::getWrapModeT() const
{
	return m_wrapModeT;
}

template<typename OutputType>
inline const PixelBuffer2D* TPixelTexture2D<OutputType>::getPixelBuffer() const
{
	PH_ASSERT(m_pixelBuffer);
	return m_pixelBuffer.get();
}

template<typename OutputType>
inline math::Vector2D TPixelTexture2D<OutputType>::sampleUVToST(const math::Vector2D& sampleUV) const
{
	return pixel_texture::uv_to_st(sampleUV, getWrapModeS(), getWrapModeT());
}

template<typename OutputType>
inline pixel_buffer::TPixel<float64> TPixelTexture2D<OutputType>::samplePixelBuffer(const math::Vector2D& sampleUV) const
{
	switch(getSampleMode())
	{
	case pixel_texture::ESampleMode::Nearest:
		return samplePixelBufferNearest(sampleUV);

	case pixel_texture::ESampleMode::Bilinear:
		return samplePixelBufferBilinear(sampleUV);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return {};
}

template<typename OutputType>
inline pixel_buffer::TPixel<float64> TPixelTexture2D<OutputType>::samplePixelBufferNearest(const math::Vector2D& sampleUV) const
{
	const math::TVector2<uint32> bufferSize = getSizePx();
	PH_ASSERT_GT(bufferSize.product(), 0);

	const math::Vector2D st = sampleUVToST(sampleUV);
		
	// Calculate pixel buffer index and handle potential overflow
	uint32 x = static_cast<uint32>(st[0] * bufferSize.x());
	uint32 y = static_cast<uint32>(st[1] * bufferSize.y());
	x = x < bufferSize.x() ? x : bufferSize.x() - 1;
	y = y < bufferSize.y() ? y : bufferSize.y() - 1;

	return getPixelBuffer()->fetchPixel({x, y}, 0);
}

template<typename OutputType>
inline pixel_buffer::TPixel<float64> TPixelTexture2D<OutputType>::samplePixelBufferBilinear(const math::Vector2D& sampleUV) const
{
	const math::TVector2<uint32> bufferSize = getSizePx();
	PH_ASSERT_GT(bufferSize.product(), 0);

	const math::Vector2D st = sampleUVToST(sampleUV);

	const float64 x  = st[0] * bufferSize.x();
	const float64 y  = st[1] * bufferSize.y();
	const float64 x0 = std::floor(x - 0.5) + 0.5;
	const float64 y0 = std::floor(y - 0.5) + 0.5;
	const float64 x1 = x0 + 1.0;
	const float64 y1 = y0 + 1.0;

	const float64 weights[4]
	{
		(x1 - x) * (y1 - y),// weight 00
		(x1 - x) * (y - y0),// weight 01
		(x - x0) * (y1 - y),// weight 10
		(x - x0) * (y - y0) // weight 11
	};
	const math::Vector2D xys[4]
	{
		{x0, y0}, {x0, y1}, {x1, y0}, {x1, y1}
	};

	using ComputePixel = math::TArithmeticArray<float64, pixel_buffer::MAX_PIXEL_ELEMENTS>;

	// Calculate bilinearly interpolated pixel by accumulating weighted samples
	ComputePixel accuPixel(0);
	for(std::size_t i = 0; i < 4; ++i)
	{
		// ST coordinates for the i-th vertex of the bilinear quad
		const math::Vector2D st_i = sampleUVToST(xys[i] * getTexelSize());

		uint32 xi = static_cast<uint32>(st_i[0] * bufferSize.x());
		uint32 yi = static_cast<uint32>(st_i[1] * bufferSize.y());
		xi = xi < bufferSize.x() ? xi : bufferSize.x() - 1;
		yi = yi < bufferSize.y() ? yi : bufferSize.y() - 1;

		const ComputePixel pixel_i(getPixelBuffer()->fetchPixel({xi, yi}, 0).getAllValues());

		accuPixel.addLocal(pixel_i * weights[i]);
	}

	return pixel_buffer::TPixel<float64>(accuPixel.toArray(), getPixelBuffer()->numPixelElements());
}

}// end namespace ph
