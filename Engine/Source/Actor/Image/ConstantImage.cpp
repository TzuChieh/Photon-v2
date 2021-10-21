#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/constant_textures.h"
#include "Math/TVector3.h"
#include "Common/logging.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Actor/actor_exceptions.h"

#include <iostream>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ConstantImage, Image);

ConstantImage::ConstantImage() :
	ConstantImage(1.0_r)
{}

ConstantImage::ConstantImage(const real value) : 
	ConstantImage(value, math::EColorSpace::UNSPECIFIED)
{}

ConstantImage::ConstantImage(const math::Vector3R& values) :
	ConstantImage(values, math::EColorSpace::UNSPECIFIED)
{}

ConstantImage::ConstantImage(std::vector<real> values) : 
	ConstantImage(values, math::EColorSpace::UNSPECIFIED)
{}

ConstantImage::ConstantImage(const real value, math::EColorSpace colorSpace) :
	ConstantImage(std::vector<real>{value}, colorSpace)
{}

ConstantImage::ConstantImage(const math::Vector3R& values, math::EColorSpace colorSpace) :
	ConstantImage(std::vector<real>{values.x, values.y, values.z}, colorSpace)
{}

ConstantImage::ConstantImage(std::vector<real> values, math::EColorSpace colorSpace) :
	m_values(std::move(values)), m_colorSpace(colorSpace)
{}

std::shared_ptr<TTexture<Image::NumericArray>> ConstantImage::genNumericTexture(
	ActorCookingContext& ctx)
{
	if(m_values.size() > Image::NUMERIC_ARRAY_SIZE)
	{
		PH_LOG_WARNING(ConstantImage, 
			"{} values provided for a numeric array of max size {}, there will be data loss",
			m_values.size(), Image::NUMERIC_ARRAY_SIZE);
	}

	Image::NumericArray arr;
	arr.fill(0);
	for(std::size_t i = 0; i < Image::NUMERIC_ARRAY_SIZE && i < m_values.size(); ++i)
	{
		arr[i] = m_values[i];
	}

	return std::make_shared<TConstantTexture<Image::NumericArray>>(arr);
}

std::shared_ptr<TTexture<math::Spectrum>> ConstantImage::genColorTexture(
	ActorCookingContext& ctx)
{
	if(m_colorSpace != math::EColorSpace::Spectral)
	{
		math::Vector3D values;
		if(m_values.size() == 1)
		{
			values.set(m_values[0]);
		}
		else if(m_values.size() == 3)
		{
			values.set(m_values[0], m_values[1], m_values[2]);
		}
		else
		{
			PH_LOG_WARNING(ConstantImage,
				"mismatched number of input values: expected 3, {} provided; generated texture may not be what you want.",
				m_values.size());

			values.x = m_values.size() >= 1 ? m_values[0] : 0;
			values.y = m_values.size() >= 2 ? m_values[1] : 0;
			values.z = m_values.size() >= 3 ? m_values[2] : 0;
		}

		const math::TristimulusValues color = math::TVector3<math::ColorValue>(values).toArray();

		switch(m_colorSpace)
		{
		case math::EColorSpace::Linear_sRGB:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::Linear_sRGB>>(color);

		case math::EColorSpace::sRGB:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::sRGB>>(color);

		case math::EColorSpace::ACEScg:
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::ACEScg>>(color);

		default:
			PH_LOG_WARNING(ConstantImage,
				"unsupported color space for tristimulus color detected; using Linear-sRGB instead",
				m_values.size());
			return std::make_shared<TConstantTristimulusTexture<math::EColorSpace::Linear_sRGB>>(color);
		}
	}
	else
	{
		math::SampledSpectrum sampledSpectrum(0);
		if(m_values.size() == 1)
		{
			sampledSpectrum = math::SampledSpectrum(static_cast<math::ColorValue>(m_values[0]));
		}
		// If there are even values, assume to be wavelength-value data points
		else if(!m_values.empty() && math::is_even(m_values.size()))
		{
			sampledSpectrum = math::SampledSpectrum(math::resample_spectral_samples<math::ColorValue, float64>(
				m_values.data(),
				m_values.data() + m_values.size() / 2,
				m_values.size() / 2));
		}
		else
		{
			throw ActorCookException(
				"invalid constant spectrum representation: "
				"(number of input values = " + std::to_string(m_values.size()) + ")");
		}

		return std::make_shared<TConstantSpectralTexture<math::EColorSpace::Spectral>>(
			sampledSpectrum);
	}
}

}// end namespace ph
