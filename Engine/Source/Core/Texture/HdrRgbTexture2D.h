#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/Texture/TAbstractPixelTex2D.h"
#include "Frame/frame_fwd.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>

namespace ph
{

class HdrRgbTexture2D : public TTexture<Spectrum>
{
public:
	HdrRgbTexture2D();
	explicit HdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> texture);

	void sample(const SampleLocation& sampleLocation, Spectrum* out_value) const override;

private:
	std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> m_texture;
};

}// end namespace ph
