#include "Engine/Actor/SDLExtension/sdl_spectrum_io.h"
#include "Engine/SDL/sdl_exceptions.h"
#include "Engine/SDL/Tokenizer.h"
#include "Engine/SDL/sdl_helpers.h"
#include "Engine/SDL/SdlInputClause.h"
#include "Engine/SDL/Introspect/TSdlEnum.h"
#include "Engine/Math/Color/color_spaces.h"
#include "Engine/Math/Color/spectral_samples.h"
#include "Engine/Math/math.h"

#include <vector>
#include <string>

namespace ph::sdl
{

math::Spectrum tristimulus_to_spectrum(
	const math::TVector3<math::ColorValue>& tristimulus,
	math::EColorSpace colorSpace,
	math::EColorUsage usage)
{
	if(usage == math::EColorUsage::Raw)
	{
		if constexpr(math::is_tristimulus(math::Spectrum::getColorSpace()))
		{
			// Set raw values directly as if in working color space, ignoring specified color space
			colorSpace = math::Spectrum::getColorSpace();
		}
		else
		{
			if(colorSpace == math::EColorSpace::Unspecified)
			{
				// For spectral space, untagged triples default to linear sRGB
				colorSpace = math::EColorSpace::Linear_sRGB;
			}
		}
	}
	else if(colorSpace == math::EColorSpace::Unspecified)
	{
		colorSpace = math::EColorSpace::Linear_sRGB;
	}

	switch(colorSpace)
	{
	case math::EColorSpace::CIE_XYZ:
		return math::Spectrum().setTransformed<math::EColorSpace::CIE_XYZ>(tristimulus.toArray(), usage);

	case math::EColorSpace::CIE_xyY:
		return math::Spectrum().setTransformed<math::EColorSpace::CIE_xyY>(tristimulus.toArray(), usage);

	case math::EColorSpace::Linear_sRGB:
		return math::Spectrum().setLinearSRGB(tristimulus.toArray(), usage);

	case math::EColorSpace::sRGB:
		return math::Spectrum().setTransformed<math::EColorSpace::sRGB>(tristimulus.toArray(), usage);

	case math::EColorSpace::ACEScg:
		return math::Spectrum().setTransformed<math::EColorSpace::ACEScg>(tristimulus.toArray(), usage);

	default:
		throw SdlLoadError("unsupported tristimulus color space conversion");
	}
}

math::Spectrum load_spectrum(
	std::string_view sdlSpectrumStr,
	std::string_view tag,
	math::EColorUsage usage)
{
	const math::EColorSpace colorSpace = TSdlEnum<math::EColorSpace>()[tag];
	return load_spectrum(sdlSpectrumStr, colorSpace, usage);
}

math::Spectrum load_spectrum(
	std::string_view sdlSpectrumStr,
	const math::EColorSpace colorSpace,
	const math::EColorUsage usage)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		// TODO: use view
		std::vector<std::string> tokens;
		tokenizer.tokenize(std::string(sdlSpectrumStr), tokens);

		// 3 input values correspond to tristimulus color
		if(tokens.size() == 3)
		{
			math::TVector3<math::ColorValue> tristimulus(
				load_number<math::ColorValue>(tokens[0]),
				load_number<math::ColorValue>(tokens[1]),
				load_number<math::ColorValue>(tokens[2]));

			return tristimulus_to_spectrum(tristimulus, colorSpace, usage);
		}
		// 1 input value represents a constant in the tagged color space
		else if(tokens.size() == 1)
		{
			const auto value = load_number<math::ColorValue>(tokens[0]);
			if(colorSpace == math::EColorSpace::Spectral)
			{
				const math::SampledSpectrum spectrum(value);
				return math::Spectrum().setSpectral(spectrum.getColorValues(), usage);
			}
			else if(usage == math::EColorUsage::Raw &&
			        colorSpace == math::EColorSpace::Unspecified)
			{
				return math::Spectrum(value);
			}

			math::TVector3<math::ColorValue> tristimulus(value);
			return tristimulus_to_spectrum(tristimulus, colorSpace, usage);
		}
		// Exact representation of a spectrum
		else if(tokens.size() == math::SampledSpectrum::NUM_VALUES &&
		        colorSpace == math::EColorSpace::Spectral)
		{
			math::SampledSpectrum spectrum;
			for(std::size_t i = 0; i < math::SampledSpectrum::NUM_VALUES; ++i)
			{
				spectrum[i] = load_number<math::ColorValue>(tokens[i]);
			}
			return math::Spectrum().setSpectral(spectrum.getColorValues(), usage);
		}
		// If there are even values, assume to be wavelength-value data points
		// (N wavelength values followed by N sample values)
		else if(!tokens.empty() && math::is_even(tokens.size()) &&
		        colorSpace == math::EColorSpace::Spectral)
		{
			const auto N = tokens.size() / 2;

			std::vector<math::ColorValue> values(N * 2);
			for(std::size_t i = 0; i < N * 2; ++i)
			{
				values[i] = load_number<math::ColorValue>(tokens[i]);
			}

			auto spectrum = math::SampledSpectrum(math::resample_spectral_samples<math::ColorValue>(
				{values.data(), N}, {values.data() + N, N}));
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
	const math::EColorUsage usage,
	std::string& out_sdlSpectrumStr,
	std::string& out_tag)
{
	try
	{
		if(usage == math::EColorUsage::Unspecified)
		{
			throw SdlSaveError("color usage must be specified");
		}

		math::EColorSpace colorSpace = math::EColorSpace::Unspecified;
		if constexpr(math::is_tristimulus(math::Spectrum::getColorSpace()))
		{
			sdl::save_number_array<math::ColorValue>(spectrum.getColorValues(), out_sdlSpectrumStr);
			colorSpace = math::Spectrum::getColorSpace();
		}
		else
		{
			// Save constant spectra as one value
			if(spectrum.minComponent() == spectrum.maxComponent())
			{
				save_number<math::ColorValue>(spectrum[0], out_sdlSpectrumStr);
			}
			// Save non-constant spectra as exact sample values.
			else
			{
				sdl::save_number_array<math::ColorValue>(spectrum.getColorValues(), out_sdlSpectrumStr);
			}
			colorSpace = math::Spectrum::getColorSpace();
		}
		out_tag += TSdlEnum<math::EColorSpace>()[colorSpace];
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Spectrum -> " + e.whatStr());
	}
}

}// end namespace ph::sdl
