#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Tokenizer.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/io_utils.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Actor/Image/HdrPictureImage.h"
#include "DataIO/SDL/SdlPayload.h"
#include "DataIO/SDL/Introspect/TSdlEnum.h"
#include "Core/Quantity/ColorSpace.h"
#include "Math/math.h"
#include "Core/Quantity/SpectralData.h"

#include <vector>

namespace ph::sdl
{

math::Vector3R tristimulus_to_linear_SRGB(const math::Vector3R& tristimulus, const ESdlColorSpace colorSpace);
{
	switch(colorSpace)
	{
	case ESdlColorSpace::UNSPECIFIED:
	case ESdlColorSpace::LINEAR_SRGB:
		return tristimulus;

	case ESdlColorSpace::SRGB:
		return ColorSpace::sRGB_to_linear_sRGB(tristimulus);

	default:
		throw SdlLoadError("unsupported tristimulus color space");
	}
}

Spectrum load_spectrum(const SdlPayload& payload, const EQuantity usage)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	const auto colorSpace = TSdlEnum<ESdlColorSpace>()[payload.tag];

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(std::string(payload.value), tokens);

		// 3 input values correspond to tristimulus color
		if(tokens.size() == 3)
		{
			const math::Vector3R tristimulus(
				load_real(tokens[0]),
				load_real(tokens[1]),
				load_real(tokens[2]));

			return Spectrum().setLinearSrgb(tristimulus_to_linear_SRGB(tristimulus, colorSpace), usage);
		}
		// 1 input value results in vec3/spectrum filled with the same value
		else if(tokens.size() == 1)
		{
			const real value = load_real(tokens[0]);
			if(colorSpace == ESdlColorSpace::SPECTRAL)
			{
				return Spectrum(value);
			}
			else
			{
				const math::Vector3R tristimulus(value);
				return Spectrum().setLinearSrgb(tristimulus_to_linear_SRGB(tristimulus, colorSpace), usage);
			}
		}
		// If there are even values, assume to be wavelength-value data points
		else if(!tokens.empty() && math::is_even(tokens.size()))
		{
			std::vector<real> values(tokens.size());
			for(std::size_t i = 0; i < values.size(); ++i)
			{
				values = load_real(tokens[i]);
			}

			const SampledSpectrum spectrum = SpectralData::calcPiecewiseAveraged(
				values.data(),
				values.data() + values.size() / 2,
				values.size() / 2);

			return Spectrum().setSampled(spectrum, usage);
		}
		else
		{
			throw SdlLoadError(
				"invalid constant representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Spectrum -> " + e.whatStr());
	}
}

//std::shared_ptr<Image> load_spectral_color(const Spectrum& spectrum)
//{
//
//}

std::shared_ptr<Image> load_tristimulus_color(const math::Vector3R& tristimulus, const ESdlColorSpace colorSpace, const EQuantity usage)
{
	return std::make_shared<ConstantImage>(
		tristimulus_to_linear_SRGB(tristimulus, colorSpace),
		usage == EQuantity::EMR
			? ConstantImage::EType::EMR_LINEAR_SRGB
			: usage == EQuantity::ECF ? ConstantImage::EType::ECF_LINEAR_SRGB : ConstantImage::EType::RAW);
}

std::shared_ptr<Image> load_picture_color(const Path& filePath)
{
	const std::string extension = filePath.getExtension();

	// TODO: what if a format supports both LDR & HDR?

	if(io_utils::has_LDR_support(extension))
	{
		return std::make_shared<LdrPictureImage>(io_utils::load_LDR_picture(filePath));
	}
	else if(io_utils::has_HDR_support(extension))
	{
		return std::make_shared<HdrPictureImage>(io_utils::load_HDR_picture(filePath));
	}
	else
	{
		throw SdlLoadError(
			"on parsing picture color -> unsupported format: " + filePath.toAbsoluteString());
	}
}

}// end namespace ph::sdl
