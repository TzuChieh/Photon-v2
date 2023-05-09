#include "Actor/SDLExtension/image_loaders.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/Tokenizer.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "SDL/sdl_helpers.h"
#include "Actor/Image/RasterFileImage.h"
#include "SDL/SdlInputClause.h"
#include "SDL/Introspect/TSdlEnum.h"
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
	case math::EColorSpace::Unspecified:
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

math::Spectrum load_spectrum(const SdlInputClause& clause, const math::EColorUsage usage)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	const auto colorSpace = TSdlEnum<math::EColorSpace>()[clause.tag];

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(std::string(clause.value), tokens);

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

			return math::Spectrum().setSpectral(spectrum.getColorValues(), usage);
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

std::shared_ptr<Image> load_tristimulus_color(const math::Vector3R& tristimulus, const math::EColorSpace colorSpace)
{
	return std::make_shared<ConstantImage>(tristimulus, colorSpace);
}

std::shared_ptr<Image> load_picture_file(const Path& filePath)
{
	return std::make_shared<RasterFileImage>(filePath);
}

}// end namespace ph::sdl
