#pragma once

#include "Core/Texture/Texture.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"

#include <vector>

namespace ph
{

template<typename ComponentType>
class TFrame;

class RgbPixelTexture final : public Texture
{
public:
	RgbPixelTexture();
	RgbPixelTexture(uint32 widthPx, uint32 heightPx);
	RgbPixelTexture(const InputPacket& packet);
	virtual ~RgbPixelTexture() override;

	virtual void sample(const Vector3R& uvw, real* out_value) const override;
	virtual void sample(const Vector3R& uvw, Vector3R* out_value) const override;
	virtual void sample(const Vector3R& uvw, SpectralStrength* out_value) const override;

	void resize(uint32 widthPx, uint32 heightPx);
	void setPixels(uint32 x, const uint32 y, uint32 widthPx, uint32 heighPx, 
	               const real* pixelData);

	// HACK
	inline void setPixels(const TFrame<uint8>& ldrFrame)
	{
		resize(ldrFrame.widthPx(), ldrFrame.heightPx());
		m_pixelData = ldrFrame.getRealData();
	}

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<real> m_pixelData;

	std::size_t calcSampleBaseIndex(const Vector3R& uvw) const;
};

}// end namespace ph