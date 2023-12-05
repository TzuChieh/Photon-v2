#include "Core/Texture/HdrRgbTexture2D.h"
#include "Core/Texture/TNearestPixelTex2D.h"

#include <Common/assertion.h>

namespace ph
{

HdrRgbTexture2D::HdrRgbTexture2D() : 
	HdrRgbTexture2D(std::make_unique<TNearestPixelTex2D<HdrComponent, 3>>())
{}

HdrRgbTexture2D::HdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> texture) :
	TTexture(),
	m_texture(std::move(texture))
{
	PH_ASSERT(m_texture);
}

void HdrRgbTexture2D::sample(const SampleLocation& sampleLocation, math::Spectrum* const out_value) const
{
	PH_ASSERT(m_texture);
	PH_ASSERT(out_value);

	TTexPixel<HdrComponent, 3> rgb;
	m_texture->sample(sampleLocation, &rgb);

	out_value->setLinearSRGB(rgb.toArray(), sampleLocation.expectedUsage());
}

}// end namespace ph
