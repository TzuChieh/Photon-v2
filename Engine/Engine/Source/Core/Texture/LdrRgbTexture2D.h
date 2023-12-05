#pragma once

#include "Core/Texture/TTexture.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TAbstractPixelTex2D.h"

#include <Common/primitive_type.h>

#include <vector>
#include <memory>

namespace ph
{

class LdrRgbTexture2D : public TTexture<math::Spectrum>
{
public:
	LdrRgbTexture2D();
	explicit LdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<LdrComponent, 3>> texture);

	void sample(const SampleLocation& sampleLocation, math::Spectrum* out_value) const override;

private:
	std::unique_ptr<TAbstractPixelTex2D<LdrComponent, 3>> m_texture;
};

}// end namespace ph
