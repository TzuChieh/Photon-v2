#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"
#include "Math/math.h"
#include "Core/Texture/Pixel/PixelBuffer2D.h"
#include "Math/TVector2.h"

#include <memory>

namespace ph
{

template<typename OutputType>
class TPixelTexture2D : public TTexture<OutputType>
{
public:
	explicit TPixelTexture2D(const std::shared_ptr<PixelBuffer2D>& pixelBuffer);

	TPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		EPixelTextureSampleMode               sampleMode,
		EPixelTextureWrapMode                 wrapMode);

	void sample(
		const SampleLocation& sampleLocation, 
		OutputType*           out_value) const override = 0;

	pixel_buffer::TPixel<float64> samplePixelBuffer(const math::Vector2D& sampleUV) const;

	math::TVector2<uint32> getSizePx() const;
	math::Vector2D getTexelSize() const;
	EPixelTextureSampleMode getSampleMode() const;
	EPixelTextureWrapMode getWrapMode() const;
	const PixelBuffer2D* getPixelBuffer() const;

	pixel_buffer::TPixel<float64> samplePixelBufferNearest(const math::Vector2D& sampleUV) const;
	pixel_buffer::TPixel<float64> samplePixelBufferBilinear(const math::Vector2D& sampleUV) const;

private:
	std::shared_ptr<PixelBuffer2D> m_pixelBuffer;
	EPixelTextureSampleMode        m_sampleMode;
	EPixelTextureWrapMode          m_wrapMode;
	math::Vector2D                 m_texelSize;
};

}// end namespace ph

#include "Core/Texture/Pixel/TPixelTexture2D.ipp"
