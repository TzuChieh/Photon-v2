#include "Core/Texture/LdrRgbTexture2D.h"
#include "Math/math.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/Texture/TextureLoader.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"
#include "Core/Texture/TNearestPixelTex2D.h"

#include <iostream>
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
	PH_ASSERT(m_texture != nullptr);
}

void LdrRgbTexture2D::sample(
	const SampleLocation& sampleLocation, SpectralStrength* const out_value) const
{
	PH_ASSERT(m_texture != nullptr && out_value != nullptr);

	TTexPixel<LdrComponent, 3> rgb255;
	m_texture->sample(sampleLocation, &rgb255);

	const Vector3R rgb(rgb255[0] / 255.0_r, 
	                   rgb255[1] / 255.0_r, 
	                   rgb255[2] / 255.0_r);
	out_value->setLinearSrgb(rgb, sampleLocation.expectedQuantity());// FIXME: check color space
}

}// end namespace ph