#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/constant_textures.h"
#include "Math/TVector3.h"
#include "Math/math.h"
#include "Actor/Basic/exceptions.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ConstantImage, Image);

std::shared_ptr<TTexture<Image::ArrayType>> ConstantImage::genNumericTexture(
	const CookingContext& ctx)
{
	if(m_values.size() > Image::ARRAY_SIZE)
	{
		PH_LOG(ConstantImage, Warning,
			"{} values provided for a numeric array of max size {}, there will be data loss.",
			m_values.size(), Image::ARRAY_SIZE);
	}

	if(m_colorSpace != math::EColorSpace::Unspecified)
	{
		PH_LOG(ConstantImage, Warning,
			"Numeric texture will ignore the specified color space ({}).",
			TSdlEnum<math::EColorSpace>{}[m_colorSpace]);
	}

	Image::ArrayType arr(0);
	for(std::size_t i = 0; i < Image::ARRAY_SIZE && i < m_values.size(); ++i)
	{
		arr[i] = m_values[i];
	}

	return std::make_shared<TConstantTexture<Image::ArrayType>>(arr);
}

std::shared_ptr<TTexture<math::Spectrum>> ConstantImage::genColorTexture(
	const CookingContext& ctx)
{
	if(math::is_tristimulus(m_colorSpace))
	{
		math::Vector3D values;
		if(m_values.size() == 0)
		{
			values.set(0);
		}
		else if(m_values.size() == 1)
		{
			values.set(m_values[0]);
		}
		else if(m_values.size() == 3)
		{
			values.set({m_values[0], m_values[1], m_values[2]});
		}
		else
		{
			PH_LOG(ConstantImage, Warning,
				"Unexpected number of input values: expected <= 3, {} provided; generated texture "
				"may not be what you want.", m_values.size());

			values.x() = m_values.size() >= 1 ? m_values[0] : 0;
			values.y() = m_values.size() >= 2 ? m_values[1] : 0;
			values.z() = m_values.size() >= 3 ? m_values[2] : 0;
		}

		const math::TristimulusValues color = math::TVector3<math::ColorValue>(values).toArray();

		// Using tristimulus texture here since we do not know how the texture will be used
		// (e.g., as energy, as raw values, etc.)
		switch(m_colorSpace)
		{
		case math::EColorSpace::CIE_XYZ:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::CIE_XYZ>>(color);

		case math::EColorSpace::CIE_xyY:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::CIE_xyY>>(color);

		case math::EColorSpace::Linear_sRGB:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::Linear_sRGB>>(color);

		case math::EColorSpace::sRGB:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::sRGB>>(color);

		case math::EColorSpace::ACEScg:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::ACEScg>>(color);

		default:
			PH_LOG(ConstantImage, Warning,
				"Unsupported color space for tristimulus color detected; using Linear-sRGB instead.");
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::Linear_sRGB>>(color);
		}
	}
	else if(m_colorSpace == math::EColorSpace::Unspecified)
	{
		if(m_values.size() == 0)
		{
			return std::make_shared<TConstantTexture<math::Spectrum>>(
				math::Spectrum(0));
		}
		else if(m_values.size() == 1)
		{
			return std::make_shared<TConstantTexture<math::Spectrum>>(
				math::Spectrum(static_cast<math::ColorValue>(m_values[0])));
		}
		// 3 input values and a direct conversion is not possible: fallback to linear sRGB
		else if(m_values.size() == 3 && m_values.size() != math::Spectrum::NUM_VALUES)
		{
			const math::Vector3D rawValues(m_values[0], m_values[1], m_values[2]);

			PH_DEBUG_LOG(ConstantImage,
				"Fallback to linear sRGB with values: {}", rawValues);

			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::Linear_sRGB>>(
				math::TVector3<math::ColorValue>(rawValues).toArray());
		}
		// Fill input values directly as much as we can
		else
		{
			if(m_values.size() != math::Spectrum::NUM_VALUES)
			{
				PH_LOG(ConstantImage, Warning,
					"Unexpected number of input values ({} provided) when treating the values as "
					"raw data; generated texture may not be what you want.", m_values.size());
			}

			math::Spectrum rawValues(0);
			for(std::size_t i = 0; i < std::min(math::Spectrum::NUM_VALUES, m_values.size()); ++i)
			{
				rawValues[i] = static_cast<math::ColorValue>(m_values[i]);
			}

			return std::make_shared<TConstantTexture<math::Spectrum>>(
				math::Spectrum(rawValues));
		}
	}
	else
	{
		math::SampledSpectrum sampledSpectrum(0);
		if(m_values.size() == 0)
		{
			sampledSpectrum = math::SampledSpectrum(0);
		}
		else if(m_values.size() == 1)
		{
			sampledSpectrum = math::SampledSpectrum(static_cast<math::ColorValue>(m_values[0]));
		}
		// Exact representation of a sampled spectrum
		else if(m_values.size() == sampledSpectrum.NUM_VALUES)
		{
			for(std::size_t i = 0; i < sampledSpectrum.NUM_VALUES; ++i)
			{
				sampledSpectrum[i] = static_cast<math::ColorValue>(m_values[i]);
			}
		}
		// If there are even values, assume to be wavelength-value data points
		// (N wavelength values followed by N sample values)
		else if(!m_values.empty() && math::is_even(m_values.size()))
		{
			sampledSpectrum = math::SampledSpectrum(math::resample_spectral_samples<math::ColorValue, float64>(
				m_values.data(),
				m_values.data() + m_values.size() / 2,
				m_values.size() / 2));
		}
		else
		{
			throw CookException(
				"invalid constant spectrum representation: "
				"(number of input values = " + std::to_string(m_values.size()) + ")");
		}

		// Using spectral texture here since we do not know how the texture will be used
		// (e.g., as energy, as raw values, etc.)
		return std::make_shared<TConstantSpectralTexture<math::EColorSpace::Spectral>>(
			sampledSpectrum.getColorValues());
	}
}

}// end namespace ph
