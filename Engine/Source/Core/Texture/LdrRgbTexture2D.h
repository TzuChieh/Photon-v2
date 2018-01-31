#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Core/Quantity/SpectralStrength.h"

#include <vector>

namespace ph
{

class LdrRgbTexture2D final : public TTexture<SpectralStrength>
{
public:
	LdrRgbTexture2D();
	LdrRgbTexture2D(uint32 widthPx, uint32 heightPx);
	virtual ~LdrRgbTexture2D() override;

	virtual void sample(const SampleLocation& sampleLocation, SpectralStrength* out_value) const override;

	void resize(uint32 widthPx, uint32 heightPx);
	void setPixels(uint32 x, const uint32 y, uint32 widthPx, uint32 heighPx, 
	               const real* pixelData);

	// HACK
	inline void setPixels(const LdrRgbFrame& ldrFrame)
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