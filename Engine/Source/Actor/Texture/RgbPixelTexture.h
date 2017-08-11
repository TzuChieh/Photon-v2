#pragma once

#include "Actor/Texture/Texture.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <vector>

namespace ph
{

class RgbPixelTexture final : public Texture
{
public:
	RgbPixelTexture();
	RgbPixelTexture(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents);
	RgbPixelTexture(const InputPacket& packet);
	virtual ~RgbPixelTexture() override;

	virtual void sample(const Vector3R& uvw, SpectralStrength* out_value) const override;

	void reset(const uint32 widthPx, const uint32 heightPx, const uint32 nPxComponents);
	void setPixels(const uint32 x, const uint32 y, const uint32 widthPx, const uint32 heighPx, const uint32 nPxComponents, 
	               const real* const pixelData);

private:
	uint32 m_widthPx;
	uint32 m_heightPx;
	uint32 m_nPxComponents;

	std::vector<real> m_pixelData;
};

}// end namespace ph