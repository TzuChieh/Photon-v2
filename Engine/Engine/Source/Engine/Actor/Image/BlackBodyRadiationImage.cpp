#include "Engine/Actor/Image/BlackBodyRadiationImage.h"
#include "Engine/Core/Texture/constant_textures.h"
#include "Engine/Math/Color/color_spaces.h"
#include "Engine/Math/Color/spectral_samples.h"
#include "Engine/Actor/Basic/exceptions.h"

#include <Common/assertion.h>
#include <Common/logging.h>

// TODO
#include "Engine/Math/TVector3.h"
#include "Engine/Math/math.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BlackBodyRadiationImage, Image);

std::shared_ptr<TTexture<Image::NumericType>> BlackBodyRadiationImage::genNumericTexture(
	const CookingContext& ctx)
{
	constexpr bool isTristimulusWorkingColorSpace =
		math::TColorSpaceDef<math::Spectrum::getColorSpace()>::isTristimulus();

	const math::Spectrum radiation = getRadiation();

	math::EColorSpace numericColorSpace;
	if(m_numericColorSpace != math::EColorSpace::Unspecified)
	{
		numericColorSpace = m_numericColorSpace;
	}
	else if constexpr(isTristimulusWorkingColorSpace)
	{
		numericColorSpace = math::Spectrum::getColorSpace();
	}
	else
	{
		numericColorSpace = math::EColorSpace::Linear_sRGB;
	}

	if(!math::is_tristimulus(numericColorSpace))
	{
		throw CookException("numeric color space cannot be spectral (too many components)");
	}

	math::TristimulusValues triValues;
	radiation.transformTo(&triValues, numericColorSpace, math::EColorUsage::EMR);

	if(triValues.size() > Image::NUMERIC_TYPE_WIDTH)
	{
		PH_LOG(BlackBodyRadiationImage, Warning,
			"{} values provided for a numeric array of max size {}, there will be data loss",
			triValues.size(), Image::NUMERIC_TYPE_WIDTH);
	}

	Image::NumericType arrayValues;
	arrayValues.set(0);
	for(std::size_t i = 0; i < Image::NUMERIC_TYPE_WIDTH && i < triValues.size(); ++i)
	{
		arrayValues[i] = triValues[i];
	}

	return std::make_shared<TConstantTexture<Image::NumericType>>(arrayValues);
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
