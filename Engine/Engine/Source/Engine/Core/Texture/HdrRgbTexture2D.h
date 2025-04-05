#pragma once

#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/Texture/TAbstractPixelTex2D.h"
#include "Engine/Frame/frame_fwd.h"
#include "Engine/Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class HdrRgbTexture2D : public TTexture<math::Spectrum>
{
public:
	HdrRgbTexture2D();
	explicit HdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> texture);

	void sample(const SampleLocation& sampleLocation, math::Spectrum* out_value) const override;

private:
	std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> m_texture;
};

}// end namespace ph
