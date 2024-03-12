#include "Actor/SDLExtension/sdl_spectrum_io.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/Tokenizer.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlInputClause.h"
#include "SDL/Introspect/TSdlEnum.h"
#include "Math/Color/color_spaces.h"
#include "Math/Color/spectral_samples.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <vector>

namespace ph::sdl
{

math::Spectrum tristimulus_to_spectrum(
	const math::TVector3<math::ColorValue>& tristimulus,
	math::EColorSpace colorSpace, 
	math::EColorUsage usage)
{
	switch(colorSpace)
	{
	case math::EColorSpace::Unspecified:
		// Set raw values directly if possible
		if constexpr(math::TColorSpaceDef<math::Spectrum::getColorSpace()>::isTristimulus())
		{
			return math::Spectrum(tristimulus.toArray());
		}
		// Non-tristimulus treat raw values as linear sRGB (as a fallback).
		else
		{
			return math::Spectrum().setLinearSRGB(tristimulus.toArray(), usage);
		}

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

math::Spectrum load_spectrum(
	const std::string& sdlSpectrumStr,
	const std::string& tag,
	math::EColorUsage usage)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	const auto colorSpace = TSdlEnum<math::EColorSpace>()[tag];

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlSpectrumStr, tokens);

		// 3 input values correspond to tristimulus color
		if(tokens.size() == 3)
		{
			math::TVector3<math::ColorValue> tristimulus(
				load_number<math::ColorValue>(tokens[0]),
				load_number<math::ColorValue>(tokens[1]),
				load_number<math::ColorValue>(tokens[2]));

			return tristimulus_to_spectrum(tristimulus, colorSpace, usage);
		}
		// 1 input value results in vec3/spectrum filled with the same value
		else if(tokens.size() == 1)
		{
			auto value = load_number<math::ColorValue>(tokens[0]);
			if(colorSpace == math::EColorSpace::Spectral)
			{
				return math::Spectrum(value);
			}
			else
			{
				math::TVector3<math::ColorValue> tristimulus(value);
				return tristimulus_to_spectrum(tristimulus, colorSpace, usage);
			}
		}
		// Exact representation of a spectrum
		else if(tokens.size() == math::Spectrum::NUM_VALUES && colorSpace == math::EColorSpace::Spectral)
		{
			math::Spectrum values;
			for(std::size_t i = 0; i < math::Spectrum::NUM_VALUES; ++i)
			{
				values[i] = load_number<math::ColorValue>(tokens[i]);
			}
			return values;
		}
		// If there are even values, assume to be wavelength-value data points
		// (N wavelength values followed by N sample values)
		else if(!tokens.empty() && math::is_even(tokens.size()))
		{
			const auto N = tokens.size() / 2;

			std::vector<math::ColorValue> values(N * 2);
			for(std::size_t i = 0; i < N * 2; ++i)
			{
				values[i] = load_number<math::ColorValue>(tokens[i]);
			}
			
			auto spectrum = math::SampledSpectrum(math::resample_spectral_samples<math::ColorValue>(
				values.data(), values.data() + N, N));
			return math::Spectrum().setSpectral(spectrum.getColorValues(), usage);
		}
		else
		{
			throw SdlLoadError(
				"invalid constant representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Spectrum -> " + e.whatStr());
	}
}

void save_spectrum(
	const math::Spectrum& spectrum,
	std::string* out_sdlSpectrumStr,
	std::string* out_tag)
{
	PH_ASSERT(out_sdlSpectrumStr);
	PH_ASSERT(out_tag);

	out_sdlSpectrumStr->clear();
	try
	{
		auto colorSpace = math::EColorSpace::Unspecified;
		if constexpr(math::TColorSpaceDef<math::Spectrum::getColorSpace()>::isTristimulus())
		{
			math::TVector3<math::ColorValue> color3(spectrum.getColorValues());
			sdl::save_vector3(color3, *out_sdlSpectrumStr);
			colorSpace = math::Spectrum::getColorSpace();
		}
		else
		{
			// Constant spectrum special case (save as a single raw value)
			if(spectrum.minComponent() == spectrum.maxComponent())
			{
				save_number<math::ColorValue>(spectrum[0], *out_sdlSpectrumStr);
				colorSpace = math::EColorSpace::Unspecified;
			}
			// Save exact representation of a spectrum (save sample values directly)
			else
			{
				sdl::save_number_array<math::ColorValue>(spectrum.getColorValues(), *out_sdlSpectrumStr);
				colorSpace = math::Spectrum::getColorSpace();
			}
		}
		*out_tag = TSdlEnum<math::EColorSpace>()[colorSpace];
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Spectrum -> " + e.whatStr());
	}
}

}// end namespace ph::sdl
