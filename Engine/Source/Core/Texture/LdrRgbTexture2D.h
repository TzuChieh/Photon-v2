#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TAbstractPixelTex2D.h"

#include <vector>
#include <memory>

namespace ph
{

class LdrRgbTexture2D final : public TTexture<SpectralStrength>
{
public:
	LdrRgbTexture2D();
	explicit LdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<LdrComponent, 3>> texture);

	void sample(const SampleLocation& sampleLocation, SpectralStrength* out_value) const override;

private:
	std::unique_ptr<TAbstractPixelTex2D<LdrComponent, 3>> m_texture;
};

}// end namespace ph