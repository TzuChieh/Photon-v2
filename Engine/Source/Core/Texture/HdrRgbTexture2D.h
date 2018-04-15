#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/Texture/TAbstractPixelTex2D.h"
#include "Frame/frame_fwd.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class HdrRgbTexture2D final : public TTexture<SpectralStrength>
{
public:
	HdrRgbTexture2D();
	HdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> texture);
	virtual ~HdrRgbTexture2D() override;

	virtual void sample(const SampleLocation& sampleLocation, SpectralStrength* out_value) const override;

private:
	std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> m_texture;
};

}// end namespace ph