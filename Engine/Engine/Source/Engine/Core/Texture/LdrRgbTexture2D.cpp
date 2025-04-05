#include "Engine/Core/Texture/LdrRgbTexture2D.h"
#include "Engine/Math/math.h"
#include "Engine/Core/Texture/TextureLoader.h"
#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Core/Texture/TNearestPixelTex2D.h"

#include <Common/assertion.h>

#include <string>

namespace ph
{

LdrRgbTexture2D::LdrRgbTexture2D() :
	LdrRgbTexture2D(std::make_unique<TNearestPixelTex2D<LdrComponent, 3>>())
{}

LdrRgbTexture2D::LdrRgbTexture2D(std::unique_ptr<TAbstractPixelTex2D<LdrComponent, 3>> texture) :
	TTexture(),
	m_texture(std::move(texture))
{
	PH_ASSERT(m_texture);
}

void LdrRgbTexture2D::sample(
	const SampleLocation& sampleLocation, math::Spectrum* const out_value) const
{
	PH_ASSERT(m_texture);
	PH_ASSERT(out_value);

	TTexPixel<LdrComponent, 3> rgb255;
	m_texture->sample(sampleLocation, &rgb255);

	const math::Vector3R rgb(
		rgb255[0] / 255.0_r,
		rgb255[1] / 255.0_r, 
		rgb255[2] / 255.0_r);
	out_value->setLinearSRGB(rgb.toArray(), sampleLocation.expectedUsage());// FIXME: check color space
}

}// end namespace ph
