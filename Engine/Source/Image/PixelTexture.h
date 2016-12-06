#pragma once

#include "Image/Texture.h"
#include "Common/primitive_type.h"

#include <vector>

namespace ph
{

class PixelTexture final : Texture
{
public:
	PixelTexture();
	PixelTexture(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents);
	virtual ~PixelTexture() override;

	virtual void sample(const Vector3f& uvw, Vector3f* const out_value) const override;

	void reset(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents);
	void setPixels(const uint32 x, const uint32 y, const uint32 widthPx, const uint32 heighPx, const uint32 nPxComponents, 
	               const float32* const pixelData);

private:
	uint32 m_widthPx;
	uint32 m_heightPx;
	uint32 m_nPxComponents;

	std::vector<float32> m_pixelData;
};

}// end namespace ph