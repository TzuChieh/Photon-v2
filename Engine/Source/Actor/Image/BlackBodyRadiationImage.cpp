#include "Actor/Image/BlackBodyRadiationImage.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Core/Texture/constant_textures.h"
#include "Math/Color/color_spaces.h"
#include "Math/Color/spectral_samples.h"
#include "Actor/Basic/exceptions.h"

// TODO
#include "Math/TVector3.h"
#include "Math/math.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BlackBodyRadiationImage, Image);

std::shared_ptr<TTexture<Image::Array>> BlackBodyRadiationImage::genNumericTexture(
	const CookingContext& ctx)
{
	constexpr bool isTristimulusMode = 
		math::TColorSpaceDef<math::Spectrum::getColorSpace()>::isTristimulus();

	const math::Spectrum radiation = getRadiation();

	math::TristimulusValues triValues;
	triValues.fill(0);
	if(m_numericColorSpace == math::EColorSpace::Unspecified)
	{
		if(isTristimulusMode)
		{
			triValues = radiation.getColorValues();
		}
		else
		{
			triValues = radiation.toLinearSRGB(math::EColorUsage::EMR);
		}
	}
	else
	{
		if(!math::is_tristimulus(m_numericColorSpace))
		{
			throw CookException(
				"numeric color space cannot be spectral (too many components)");
		}

		radiation.transformTo(&triValues, m_numericColorSpace, math::EColorUsage::EMR);
	}

	if(triValues.size() > Image::ARRAY_SIZE)
	{
		PH_LOG_WARNING(BlackBodyRadiationImage,
			"{} values provided for a numeric array of max size {}, there will be data loss",
			triValues.size(), Image::ARRAY_SIZE);
	}

	Image::Array arrayValues;
	arrayValues.set(0);
	for(std::size_t i = 0; i < Image::ARRAY_SIZE && i < triValues.size(); ++i)
	{
		arrayValues[i] = triValues[i];
	}

	return std::make_shared<TConstantTexture<Image::Array>>(arrayValues);
}

std::shared_ptr<TTexture<math::Spectrum>> BlackBodyRadiationImage::genColorTexture(
	const CookingContext& ctx)
{
	// Using constant texture here since we already know the data represents energy source
	//
	return std::make_shared<TConstantTexture<math::Spectrum>>(
		getRadiation());
}

math::Spectrum BlackBodyRadiationImage::getRadiation() const
{
	using ColorT = math::ColorValue;

	math::Spectrum radiation(0);
	if(m_energy)
	{
		auto samples = math::resample_black_body<ColorT>(m_temperatureK);
		samples = math::put_color_energy<math::EColorSpace::Spectral, ColorT>(samples, *m_energy);
		
		radiation.setSpectral(samples, math::EColorUsage::EMR);
	}
	else
	{
		if(m_isSpectralRadiance)
		{
			radiation.setSpectral(
				math::resample_black_body_spectral_radiance<ColorT>(m_temperatureK), 
				math::EColorUsage::EMR);
		}
		else
		{
			radiation.setSpectral(
				math::resample_black_body_radiance<ColorT>(m_temperatureK),
				math::EColorUsage::EMR);
		}
	}

	return radiation;
}

}// end namespace ph
