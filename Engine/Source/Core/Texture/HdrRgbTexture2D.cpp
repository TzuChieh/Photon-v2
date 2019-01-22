#include "Core/Texture/HdrRgbTexture2D.h"
#include "Common/assertion.h"
#include "Core/Texture/TNearestPixelTex2D.h"

namespace ph
{

HdrRgbTexture2D::HdrRgbTexture2D() : 
	HdrRgbTexture2D(std::make_unique<TNearestPixelTex2D<HdrComponent, 3>>())
{}

HdrRgbTexture2D::HdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> texture) :
	TTexture(),
	m_texture(std::move(texture))
{
	PH_ASSERT(m_texture != nullptr);
}

void HdrRgbTexture2D::sample(const SampleLocation& sampleLocation, SpectralStrength* const out_value) const
{
	PH_ASSERT(m_texture != nullptr && out_value != nullptr);

	TTexPixel<HdrComponent, 3> rgb;
	m_texture->sample(sampleLocation, &rgb);

	out_value->setLinearSrgb(Vector3R(rgb[0], rgb[1], rgb[2]), sampleLocation.expectedQuantity());
}

}// end namespace ph