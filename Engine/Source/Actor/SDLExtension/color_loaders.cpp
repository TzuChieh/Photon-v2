#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Tokenizer.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/io_utils.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Actor/Image/HdrPictureImage.h"
#include "DataIO/SDL/SdlInputPayload.h"
#include "DataIO/SDL/Introspect/TSdlEnum.h"
#include "Math/Color/color_spaces.h"
#include "Math/Color/spectral_samples.h"
#include "Math/math.h"

#include <vector>

namespace ph::sdl
{

math::Spectrum tristimulus_to_spectrum(const math::Vector3R& tristimulus, const math::EColorSpace colorSpace, const math::EColorUsage usage)
{
	switch(colorSpace)
	{
	case math::EColorSpace::UNSPECIFIED:
	case math::EColorSpace::Linear_sRGB:
		return math::Spectrum().setLinearSRGB(tristimulus.toArray(), usage);

	case math::EColorSpace::sRGB:
		return math::Spectrum().setTransformed<math::EColorSpace::sRGB>(tristimulus.toArray(), usage);

	case math::EColorSpace::ACEScg:
		return math::Spectrum().setTransformed<math::EColorSpace::ACEScg>(tristimulus.toArray(), usage);

	default:
		throw SdlLoadError("unsupported tristimulus color space");
	}
}

math::Spectrum load_spectrum(const SdlInputPayload& payload, const math::EColorUsage usage)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	const auto colorSpace = TSdlEnum<math::EColorSpace>()[payload.tag];

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

			return tristimulus_to_spectrum(tristimulus, colorSpace, usage);
		}
		// 1 input value results in vec3/spectrum filled with the same value
		else if(tokens.size() == 1)
		{
			const real value = load_real(tokens[0]);
			if(colorSpace == math::EColorSpace::Spectral)
			{
				return math::Spectrum(value);
			}
			else
			{
				const math::Vector3R tristimulus(value);
				return tristimulus_to_spectrum(tristimulus, colorSpace, usage);
			}
		}
		// If there are even values, assume to be wavelength-value data points
		else if(!tokens.empty() && math::is_even(tokens.size()))
		{
			std::vector<real> values(tokens.size());
			for(std::size_t i = 0; i < values.size(); ++i)
			{
				values[i] = load_real(tokens[i]);
			}
			
			const auto spectrum = math::SampledSpectrum(math::resample_spectral_samples<math::ColorValue, real>(
				values.data(),
				values.data() + values.size() / 2,
				values.size() / 2));

			return math::Spectrum().setTransformed<math::EColorSpace::Spectral>(spectrum, usage);
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

std::shared_ptr<Image> load_tristimulus_color(const math::Vector3R& tristimulus, const math::EColorSpace colorSpace, const math::EColorUsage usage)
{
	return std::make_shared<ConstantImage>(
		math::Vector3R(tristimulus_to_spectrum(tristimulus, colorSpace, usage).toLinearSRGB(usage)),
		usage == math::EColorUsage::EMR
			? ConstantImage::EType::EMR_LINEAR_SRGB
			: usage == math::EColorUsage::ECF ? ConstantImage::EType::ECF_LINEAR_SRGB : ConstantImage::EType::RAW);
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
